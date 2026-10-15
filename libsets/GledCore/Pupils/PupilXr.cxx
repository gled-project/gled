// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "PupilXr.h"
#include "Pupil.h"
#include <Glasses/PupilInfo.h>

#include <TMath.h>

#include <cstdio>

#if __has_include(<openxr/openxr.h>)

#include <GL/glew.h>
#include <GL/glxew.h>
#include <FL/Fl.H>
#include <FL/x.H>

#define XR_USE_GRAPHICS_API_OPENGL
#define XR_USE_PLATFORM_XLIB
#include <openxr/openxr.h>
#include <openxr/openxr_platform.h>

#include <cmath>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <unistd.h>

using namespace gled;

struct PupilXr::Impl
{
  struct Eye
  {
    XrSwapchain                            fSwapchain = XR_NULL_HANDLE;
    Int_t                                  fW = 0, fH = 0;
    std::vector<XrSwapchainImageOpenGLKHR> fImages;
    GLuint                                 fFBO = 0, fDepth = 0;
  };

  Pupil          *fPupil;
  XrInstance      fInstance = XR_NULL_HANDLE;
  XrSystemId      fSystem   = XR_NULL_SYSTEM_ID;
  XrSession       fSession  = XR_NULL_HANDLE;
  XrSpace         fSpace    = XR_NULL_HANDLE;
  bool            bRunning  = false;
  bool            bExit     = false;
  Eye             fEyes[2];
  XrView          fViews[2];
  Double_t        fScale    = 0.2;  // Metres per gled unit.
  Long64_t        fFrames   = 0;

  bool ok(XrResult r, const char* what)
  {
    if (XR_SUCCEEDED(r)) return true;
    char s[XR_MAX_RESULT_STRING_SIZE] = "?";
    if (fInstance) xrResultToString(fInstance, r, s);
    printf("PupilXr: %s failed: %s (%d).\n", what, s, (int) r);
    return false;
  }
};

//==============================================================================

PupilXr::PupilXr(Pupil* pupil) : m(new Impl)
{
  m->fPupil = pupil;
  for (auto &v : m->fViews) v = { XR_TYPE_VIEW };
  if (const char* s = getenv("GLED_XR_SCALE"))
    m->fScale = atof(s);
}

PupilXr::~PupilXr()
{
  Fl::remove_idle(IdleCallback, this);
  m->fPupil->make_current();
  for (auto &e : m->fEyes)
  {
    if (e.fFBO)   glDeleteFramebuffers(1, &e.fFBO);
    if (e.fDepth) glDeleteRenderbuffers(1, &e.fDepth);
    if (e.fSwapchain) xrDestroySwapchain(e.fSwapchain);
  }
  if (m->fSpace)    xrDestroySpace(m->fSpace);
  if (m->fSession)  xrDestroySession(m->fSession);
  if (m->fInstance) xrDestroyInstance(m->fInstance);
  delete m;
}

bool PupilXr::Init()
{
  Impl &M = *m;

  const char* exts[] = { XR_KHR_OPENGL_ENABLE_EXTENSION_NAME };
  XrInstanceCreateInfo ici { XR_TYPE_INSTANCE_CREATE_INFO };
  strcpy(ici.applicationInfo.applicationName, "gled");
  ici.applicationInfo.apiVersion = XR_API_VERSION_1_0;
  ici.enabledExtensionCount      = 1;
  ici.enabledExtensionNames      = exts;
  if ( ! M.ok(xrCreateInstance(&ici, &M.fInstance), "xrCreateInstance")) return false;

  XrSystemGetInfo sgi { XR_TYPE_SYSTEM_GET_INFO };
  sgi.formFactor = XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY;
  if ( ! M.ok(xrGetSystem(M.fInstance, &sgi, &M.fSystem), "xrGetSystem")) return false;

  // Required before xrCreateSession.
  PFN_xrGetOpenGLGraphicsRequirementsKHR get_reqs = 0;
  xrGetInstanceProcAddr(M.fInstance, "xrGetOpenGLGraphicsRequirementsKHR", (PFN_xrVoidFunction*) &get_reqs);
  XrGraphicsRequirementsOpenGLKHR reqs { XR_TYPE_GRAPHICS_REQUIREMENTS_OPENGL_KHR };
  if ( ! get_reqs || ! M.ok(get_reqs(M.fInstance, M.fSystem, &reqs), "xrGetOpenGLGraphicsRequirementsKHR"))
    return false;

  // The pupil's context, from FLTK.
  M.fPupil->make_current();
  Display     *dpy = fl_display;
  GLXContext   ctx = glXGetCurrentContext();
  GLXDrawable  drw = glXGetCurrentDrawable();
  int fb_id = 0, n_cfg = 0, vis_id = 0;
  glXQueryContext(dpy, ctx, GLX_FBCONFIG_ID, &fb_id);
  int          attrs[] = { GLX_FBCONFIG_ID, fb_id, None };
  GLXFBConfig *cfgs = glXChooseFBConfig(dpy, DefaultScreen(dpy), attrs, &n_cfg);
  GLXFBConfig  cfg  = n_cfg > 0 ? cfgs[0] : 0;
  if (cfg) glXGetFBConfigAttrib(dpy, cfg, GLX_VISUAL_ID, &vis_id);
  if (cfgs) XFree(cfgs);

  XrGraphicsBindingOpenGLXlibKHR gb { XR_TYPE_GRAPHICS_BINDING_OPENGL_XLIB_KHR };
  gb.xDisplay    = dpy;
  gb.visualid    = vis_id;
  gb.glxFBConfig = cfg;
  gb.glxDrawable = drw;
  gb.glxContext  = ctx;
  XrSessionCreateInfo sci { XR_TYPE_SESSION_CREATE_INFO };
  sci.next     = &gb;
  sci.systemId = M.fSystem;
  if ( ! M.ok(xrCreateSession(M.fInstance, &sci, &M.fSession), "xrCreateSession")) return false;

  XrReferenceSpaceCreateInfo rsci { XR_TYPE_REFERENCE_SPACE_CREATE_INFO };
  rsci.referenceSpaceType = XR_REFERENCE_SPACE_TYPE_LOCAL;
  rsci.poseInReferenceSpace.orientation.w = 1;
  if ( ! M.ok(xrCreateReferenceSpace(M.fSession, &rsci, &M.fSpace), "xrCreateReferenceSpace")) return false;

  uint32_t n_views = 0;
  XrViewConfigurationView vcv[2] = { { XR_TYPE_VIEW_CONFIGURATION_VIEW }, { XR_TYPE_VIEW_CONFIGURATION_VIEW } };
  if ( ! M.ok(xrEnumerateViewConfigurationViews(M.fInstance, M.fSystem, XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO,
                                                2, &n_views, vcv), "xrEnumerateViewConfigurationViews"))
    return false;

  // sRGB without GL_FRAMEBUFFER_SRGB: the colours look as on the desktop.
  uint32_t n_fmt = 0;
  xrEnumerateSwapchainFormats(M.fSession, 0, &n_fmt, 0);
  std::vector<int64_t> fmts(n_fmt);
  xrEnumerateSwapchainFormats(M.fSession, n_fmt, &n_fmt, fmts.data());
  int64_t fmt = fmts.empty() ? GL_RGBA8 : fmts[0];
  for (int64_t want : { (int64_t) GL_SRGB8_ALPHA8, (int64_t) GL_RGBA8 })
  {
    bool found = false;
    for (int64_t f : fmts) if (f == want) found = true;
    if (found) { fmt = want; break; }
  }

  for (int e = 0; e < 2; ++e)
  {
    Impl::Eye &E = M.fEyes[e];
    E.fW = vcv[e].recommendedImageRectWidth;
    E.fH = vcv[e].recommendedImageRectHeight;

    XrSwapchainCreateInfo sc { XR_TYPE_SWAPCHAIN_CREATE_INFO };
    sc.usageFlags  = XR_SWAPCHAIN_USAGE_COLOR_ATTACHMENT_BIT | XR_SWAPCHAIN_USAGE_SAMPLED_BIT;
    sc.format      = fmt;
    sc.sampleCount = 1;
    sc.width       = E.fW;
    sc.height      = E.fH;
    sc.faceCount   = 1;
    sc.arraySize   = 1;
    sc.mipCount    = 1;
    if ( ! M.ok(xrCreateSwapchain(M.fSession, &sc, &E.fSwapchain), "xrCreateSwapchain")) return false;

    uint32_t n_img = 0;
    xrEnumerateSwapchainImages(E.fSwapchain, 0, &n_img, 0);
    E.fImages.assign(n_img, { XR_TYPE_SWAPCHAIN_IMAGE_OPENGL_KHR });
    xrEnumerateSwapchainImages(E.fSwapchain, n_img, &n_img, (XrSwapchainImageBaseHeader*) E.fImages.data());

    glGenFramebuffers(1, &E.fFBO);
    glGenRenderbuffers(1, &E.fDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, E.fDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, E.fW, E.fH);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
  }

  XrInstanceProperties ip { XR_TYPE_INSTANCE_PROPERTIES };
  xrGetInstanceProperties(M.fInstance, &ip);
  printf("PupilXr: %s, %dx%d per eye, format 0x%llx, %.3g m per unit.\n", ip.runtimeName,
         M.fEyes[0].fW, M.fEyes[0].fH, (unsigned long long) fmt, M.fScale);
  return true;
}

//==============================================================================

bool PupilXr::poll_events()
{
  Impl &M = *m;
  XrEventDataBuffer ev { XR_TYPE_EVENT_DATA_BUFFER };
  while (xrPollEvent(M.fInstance, &ev) == XR_SUCCESS)
  {
    if (ev.type == XR_TYPE_EVENT_DATA_SESSION_STATE_CHANGED)
    {
      XrSessionState s = ((XrEventDataSessionStateChanged*) &ev)->state;
      printf("PupilXr: session state %d.\n", (int) s);
      if (s == XR_SESSION_STATE_READY)
      {
        XrSessionBeginInfo bi { XR_TYPE_SESSION_BEGIN_INFO };
        bi.primaryViewConfigurationType = XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO;
        M.bRunning = M.ok(xrBeginSession(M.fSession, &bi), "xrBeginSession");
      }
      else if (s == XR_SESSION_STATE_STOPPING)
      {
        xrEndSession(M.fSession);
        M.bRunning = false;
      }
      else if (s == XR_SESSION_STATE_EXITING || s == XR_SESSION_STATE_LOSS_PENDING)
      {
        M.bExit = true;
      }
    }
    else if (ev.type == XR_TYPE_EVENT_DATA_INSTANCE_LOSS_PENDING)
    {
      M.bExit = true;
    }
    ev = { XR_TYPE_EVENT_DATA_BUFFER };
  }
  return ! M.bExit;
}

void PupilXr::render_eye(int e)
{
  Impl      &M = *m;
  Impl::Eye &E = M.fEyes[e];
  Pupil     &P = *M.fPupil;

  uint32_t idx = 0;
  XrSwapchainImageAcquireInfo ai { XR_TYPE_SWAPCHAIN_IMAGE_ACQUIRE_INFO };
  if ( ! M.ok(xrAcquireSwapchainImage(E.fSwapchain, &ai, &idx), "xrAcquireSwapchainImage")) return;
  XrSwapchainImageWaitInfo wi { XR_TYPE_SWAPCHAIN_IMAGE_WAIT_INFO };
  wi.timeout = XR_INFINITE_DURATION;
  xrWaitSwapchainImage(E.fSwapchain, &wi);

  glBindFramebuffer(GL_FRAMEBUFFER, E.fFBO);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, E.fImages[idx].image, 0);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, E.fDepth);
  glViewport(0, 0, E.fW, E.fH);

  const XrFovf &fov = M.fViews[e].fov;
  Double_t n = TMath::Max(P.mInfo->GetNearClip(), 1e-5f), f = P.mInfo->GetFarClip();
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glFrustum(n*std::tan(fov.angleLeft), n*std::tan(fov.angleRight),
            n*std::tan(fov.angleDown), n*std::tan(fov.angleUp), n, f);

  // OpenXR's view space has GL's eye axes. The inverse eye pose, in gled
  // units, goes before the camera, so the headset starts at the pupil's camera.
  const XrQuaternionf &q = M.fViews[e].pose.orientation;
  const XrVector3f    &p = M.fViews[e].pose.position;
  const Double_t x = q.x, y = q.y, z = q.z, w = q.w;
  const Double_t R[3][3] = { { 1 - 2*(y*y + z*z), 2*(x*y - z*w),     2*(x*z + y*w)     },
                             { 2*(x*y + z*w),     1 - 2*(x*x + z*z), 2*(y*z - x*w)     },
                             { 2*(x*z - y*w),     2*(y*z + x*w),     1 - 2*(x*x + y*y) } };
  const Double_t t[3] = { p.x / M.fScale, p.y / M.fScale, p.z / M.fScale };
  Double_t mv[16] = { 0 };
  for (int r = 0; r < 3; ++r)
  {
    for (int c = 0; c < 3; ++c) mv[4*c + r] = R[c][r];
    mv[12 + r] = -(R[0][r]*t[0] + R[1][r]*t[1] + R[2][r]*t[2]);
  }
  mv[15] = 1;
  glMatrixMode(GL_MODELVIEW);
  glLoadMatrixd(mv);
  try
  {
    P.SetAbsRelCamera();
    P.SetCameraView();
  }
  catch (Exc_t& exc)
  {
    printf("PupilXr: camera setup failed: %s.\n", exc.Data());
  }

  P.rnr_default_init();
  P.Render(true, false);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  XrSwapchainImageReleaseInfo ri { XR_TYPE_SWAPCHAIN_IMAGE_RELEASE_INFO };
  xrReleaseSwapchainImage(E.fSwapchain, &ri);
}

void PupilXr::frame()
{
  Impl &M = *m;

  if ( ! poll_events())
  {
    printf("PupilXr: session ended after %lld frames.\n", M.fFrames);
    Fl::remove_idle(IdleCallback, this);
    return;
  }
  if ( ! M.bRunning)
  {
    usleep(10000);
    return;
  }

  M.fPupil->make_current();

  XrFrameWaitInfo wi { XR_TYPE_FRAME_WAIT_INFO };
  XrFrameState    fs { XR_TYPE_FRAME_STATE };
  if ( ! M.ok(xrWaitFrame(M.fSession, &wi, &fs), "xrWaitFrame")) return;
  XrFrameBeginInfo bi { XR_TYPE_FRAME_BEGIN_INFO };
  if ( ! M.ok(xrBeginFrame(M.fSession, &bi), "xrBeginFrame")) return;

  XrCompositionLayerProjectionView pv[2];
  XrCompositionLayerProjection     layer { XR_TYPE_COMPOSITION_LAYER_PROJECTION };
  const XrCompositionLayerBaseHeader *layers[1];
  uint32_t n_layers = 0;

  if (fs.shouldRender)
  {
    XrViewLocateInfo li { XR_TYPE_VIEW_LOCATE_INFO };
    li.viewConfigurationType = XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO;
    li.displayTime           = fs.predictedDisplayTime;
    li.space                 = M.fSpace;
    XrViewState vs { XR_TYPE_VIEW_STATE };
    uint32_t    n_views = 0;
    M.ok(xrLocateViews(M.fSession, &li, &vs, 2, &n_views, M.fViews), "xrLocateViews");

    const XrViewStateFlags valid = XR_VIEW_STATE_POSITION_VALID_BIT | XR_VIEW_STATE_ORIENTATION_VALID_BIT;
    if (n_views == 2 && (vs.viewStateFlags & valid) == valid)
    {
      for (int e = 0; e < 2; ++e)
      {
        // Every xrBeginFrame needs its xrEndFrame.
        try { render_eye(e); }
        catch (Exc_t& exc) { printf("PupilXr: render failed: %s.\n", exc.Data()); }

        pv[e] = { XR_TYPE_COMPOSITION_LAYER_PROJECTION_VIEW };
        pv[e].pose                      = M.fViews[e].pose;
        pv[e].fov                       = M.fViews[e].fov;
        pv[e].subImage.swapchain        = M.fEyes[e].fSwapchain;
        pv[e].subImage.imageRect.extent = { M.fEyes[e].fW, M.fEyes[e].fH };
      }
      layer.space     = M.fSpace;
      layer.viewCount = 2;
      layer.views     = pv;
      layers[n_layers++] = (const XrCompositionLayerBaseHeader*) &layer;
    }
  }

  XrFrameEndInfo ei { XR_TYPE_FRAME_END_INFO };
  ei.displayTime          = fs.predictedDisplayTime;
  ei.environmentBlendMode = XR_ENVIRONMENT_BLEND_MODE_OPAQUE;
  ei.layerCount           = n_layers;
  ei.layers               = layers;
  M.ok(xrEndFrame(M.fSession, &ei), "xrEndFrame");
  ++M.fFrames;
}

void PupilXr::IdleCallback(void* pxr)
{
  ((PupilXr*) pxr)->frame();
}

#else

using namespace gled;

struct PupilXr::Impl {};

PupilXr::PupilXr(Pupil* pupil) : m(0) {}
PupilXr::~PupilXr() {}

bool PupilXr::Init()
{
  printf("PupilXr: gled was built without OpenXR.\n");
  return false;
}

void PupilXr::IdleCallback(void* pxr) {}

#endif
