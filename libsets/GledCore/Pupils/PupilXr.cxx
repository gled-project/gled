// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "PupilXr.h"
#include "Pupil.h"
#include <Glasses/PupilInfo.h>
#include <RnrBase/XrRig.h>
#include <Rnr/GL/GLRnrDriver.h>
#include <Gled/GTime.h>

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

namespace
{
  const XrSpaceLocationFlags kPoseValid =
    XR_SPACE_LOCATION_POSITION_VALID_BIT | XR_SPACE_LOCATION_ORIENTATION_VALID_BIT;

  ZTrans pose_to_trans(const XrPosef& pose)
  {
    const XrQuaternionf &q = pose.orientation;
    const Double_t x = q.x, y = q.y, z = q.z, w = q.w;
    const Double_t R[3][3] = { { 1 - 2*(y*y + z*z), 2*(x*y - z*w),     2*(x*z + y*w)     },
                               { 2*(x*y + z*w),     1 - 2*(x*x + z*z), 2*(y*z - x*w)     },
                               { 2*(x*z - y*w),     2*(y*z + x*w),     1 - 2*(x*x + y*y) } };
    ZTrans t;
    for (int i = 0; i < 3; ++i)
      for (int j = 0; j < 3; ++j)
        t.CM(i, j) = R[i][j];
    t.CM(0, 3) = pose.position.x;
    t.CM(1, 3) = pose.position.y;
    t.CM(2, 3) = pose.position.z;
    return t;
  }

  ZPoint pos_of(const ZTrans& t) { return ZPoint(t.CM(0, 3), t.CM(1, 3), t.CM(2, 3)); }
  ZPoint axis_of(const ZTrans& t, int i) { return ZPoint(t.CM(0, i), t.CM(1, i), t.CM(2, i)); }

  // A texture to render into, with depth if asked for.
  struct Target
  {
    GLuint fFBO = 0, fTex = 0, fDepth = 0;
    Int_t  fW = 0, fH = 0;

    void ensure(Int_t w, Int_t h, bool depth)
    {
      if (fFBO && w == fW && h == fH) return;
      destroy();
      fW = w; fH = h;
      glGenTextures(1, &fTex);
      glBindTexture(GL_TEXTURE_2D, fTex);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      glBindTexture(GL_TEXTURE_2D, 0);
      glGenFramebuffers(1, &fFBO);
      glBindFramebuffer(GL_FRAMEBUFFER, fFBO);
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fTex, 0);
      if (depth)
      {
        glGenRenderbuffers(1, &fDepth);
        glBindRenderbuffer(GL_RENDERBUFFER, fDepth);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, w, h);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, fDepth);
      }
      glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void destroy()
    {
      if (fFBO)   glDeleteFramebuffers(1, &fFBO);
      if (fTex)   glDeleteTextures(1, &fTex);
      if (fDepth) glDeleteRenderbuffers(1, &fDepth);
      fFBO = fTex = fDepth = 0;
      fW = fH = 0;
    }
  };
}

// The controllers in one frame: poses in the reference space, buttons, sticks.
struct PupilXr::Input
{
  bool    bFocused      = false;
  ZTrans  fGrip[2];
  bool    bGripValid[2] = { false, false };
  bool    bGrab[2]      = { false, false };
  Float_t fStick[4]     = { 0, 0, 0, 0 };    // left x, y, right x, y
  ZTrans  fAim;
  bool    bAimValid     = false;
  bool    bTrigger      = false, bTriggerChanged = false;
  bool    bShoot        = false, bShootChanged   = false;
  bool    bExit         = false, bRecenter = false, bHome = false, bPanelToggle = false;
};

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

  // The session.
  XrInstance      fInstance = XR_NULL_HANDLE;
  XrSystemId      fSystem   = XR_NULL_SYSTEM_ID;
  XrSession       fSession  = XR_NULL_HANDLE;
  XrSpace         fSpace    = XR_NULL_HANDLE;
  bool            bRunning  = false;
  bool            bExit     = false;
  GLXDrawable     fDrawable = 0;
  Eye             fEyes[2];
  XrView          fViews[2];
  bool            bViewsOk  = false;
  XrTime          fLastTime = 0;

  // Per frame, waiting in xrWaitFrame, rendering and submitting, and the time
  // between frames, when FLTK runs the desktop pupil and the Eye.
  Long64_t        fFrames   = 0;
  GTime           fRateStart, fFrameEnd;
  Double_t        fWaitSum = 0, fWorkSum = 0, fGapSum = 0;
  XrDuration      fPeriod = 0;

  // The controllers: left and right subactions.
  XrActionSet     fActionSet = XR_NULL_HANDLE;
  XrAction        fGripPose = XR_NULL_HANDLE, fGrab = XR_NULL_HANDLE, fStick = XR_NULL_HANDLE,
                  fAimPose = XR_NULL_HANDLE, fTrigger = XR_NULL_HANDLE, fShoot = XR_NULL_HANDLE,
                  fRecenter = XR_NULL_HANDLE, fExitAct = XR_NULL_HANDLE, fPanelAct = XR_NULL_HANDLE,
                  fBuzz = XR_NULL_HANDLE;
  XrPath          fHand[2]      = { XR_NULL_PATH, XR_NULL_PATH };
  XrSpace         fHandSpace[2] = { XR_NULL_HANDLE, XR_NULL_HANDLE };
  XrSpace         fAimSpace     = XR_NULL_HANDLE;
  bool            bGrabPrev[2]  = { false, false };
  bool            bTriggerPrev  = false;
  XrTime          fADown        = 0;     // When A went down, 0 once its press is used.

  // The placement, the ridden frame's view and the camera base, the head.
  XrRig           fRig;
  ZTrans          fLook, fBase, fHead;

  // The laser of this frame, and its press on the panel or in the scene.
  ZTrans          fAim;
  bool            bAimValid = false;
  Double_t        fLaserLen = 30;
  bool            bPanelPress = false, bScenePress = false;

  // The overlay panel above the left hand: the box of window pixels the
  // overlay draws in (GL's y up), found from the texture's alpha every few
  // frames; its frame (z toward the head) and half sizes in metres; and where
  // the laser is on it, in window pixels (FLTK's y down).
  Target          fPanel;
  bool            bPanelOn = true, bPanelShown = false;
  Int_t           fBox[4] = { 0, 0, 0, 0 };   // x0, y0, x1, y1
  Long64_t        fBoxFrame = -1000;
  ZTrans          fPanelFrame;
  Double_t        fPanelHW = 0, fPanelHH = 0;
  bool            bPanelHit = false;
  Int_t           fPanelX = 0, fPanelY = 0;

  // The pupil's window mirrors the left eye.
  bool            bMirror = true;
  Target          fMirror;

  bool ok(XrResult r, const char* what)
  {
    if (XR_SUCCEEDED(r)) return true;
    char s[XR_MAX_RESULT_STRING_SIZE] = "?";
    if (fInstance) xrResultToString(fInstance, r, s);
    printf("PupilXr: %s failed: %s (%d).\n", what, s, (int) r);
    return false;
  }

  bool make_action(XrActionType type, const char* name, const char* label, XrAction* a)
  {
    XrActionCreateInfo aci { XR_TYPE_ACTION_CREATE_INFO };
    aci.actionType          = type;
    aci.countSubactionPaths = 2;
    aci.subactionPaths      = fHand;
    strcpy(aci.actionName, name);
    strcpy(aci.localizedActionName, label);
    return ok(xrCreateAction(fActionSet, &aci, a), "xrCreateAction");
  }

  bool suggest(const char* profile, const std::vector<std::pair<XrAction, const char*>>& binds)
  {
    std::vector<XrActionSuggestedBinding> sb;
    for (auto &b : binds)
    {
      XrPath p;
      xrStringToPath(fInstance, b.second, &p);
      sb.push_back({ b.first, p });
    }
    XrInteractionProfileSuggestedBinding ipsb { XR_TYPE_INTERACTION_PROFILE_SUGGESTED_BINDING };
    xrStringToPath(fInstance, profile, &ipsb.interactionProfile);
    ipsb.countSuggestedBindings = sb.size();
    ipsb.suggestedBindings      = sb.data();
    return ok(xrSuggestInteractionProfileBindings(fInstance, &ipsb), profile);
  }

  bool make_space(XrAction pose, int hand, XrSpace* s)
  {
    XrActionSpaceCreateInfo ascr { XR_TYPE_ACTION_SPACE_CREATE_INFO };
    ascr.action        = pose;
    ascr.subactionPath = fHand[hand];
    ascr.poseInActionSpace.orientation.w = 1;
    return ok(xrCreateActionSpace(fSession, &ascr, s), "xrCreateActionSpace");
  }

  XrActionStateGetInfo get_info(XrAction a, int hand)
  {
    XrActionStateGetInfo gi { XR_TYPE_ACTION_STATE_GET_INFO };
    gi.action        = a;
    gi.subactionPath = fHand[hand];
    return gi;
  }

  // A button that went down since the last sync.
  bool pressed(XrAction a, int hand)
  {
    XrActionStateGetInfo gi = get_info(a, hand);
    XrActionStateBoolean s { XR_TYPE_ACTION_STATE_BOOLEAN };
    xrGetActionStateBoolean(fSession, &gi, &s);
    return s.isActive && s.changedSinceLastSync && s.currentState;
  }

  // A squeeze or trigger past 0.6, held until it drops below 0.4.
  bool held(XrAction a, int hand, bool was)
  {
    XrActionStateGetInfo gi = get_info(a, hand);
    XrActionStateFloat s { XR_TYPE_ACTION_STATE_FLOAT };
    xrGetActionStateFloat(fSession, &gi, &s);
    return s.isActive && s.currentState > (was ? 0.4f : 0.6f);
  }

  bool locate(XrSpace space, XrTime t, ZTrans& pose)
  {
    XrSpaceLocation loc { XR_TYPE_SPACE_LOCATION };
    xrLocateSpace(space, fSpace, t, &loc);
    if ((loc.locationFlags & kPoseValid) != kPoseValid) return false;
    pose = pose_to_trans(loc.pose);
    return true;
  }

  void buzz(int hand)
  {
    XrHapticVibration v { XR_TYPE_HAPTIC_VIBRATION };
    v.amplitude = 0.4f;
    v.duration  = 15000000; // ns
    v.frequency = XR_FREQUENCY_UNSPECIFIED;
    XrHapticActionInfo hi { XR_TYPE_HAPTIC_ACTION_INFO };
    hi.action        = fBuzz;
    hi.subactionPath = fHand[hand];
    xrApplyHapticFeedback(fSession, &hi, (XrHapticBaseHeader*) &v);
  }
};

//==============================================================================

PupilXr::PupilXr(Pupil* pupil) : m(new Impl)
{
  m->fPupil = pupil;
  for (auto &v : m->fViews) v = { XR_TYPE_VIEW };
  if (const char* s = getenv("GLED_XR_SCALE")) m->fRig = XrRig(atof(s));
  if (const char* s = getenv("GLED_XR_FLY"))   m->fRig.fFlyAccel = atof(s);
  if (const char* s = getenv("GLED_XR_LEVEL")) m->fRig.bLevel    = atoi(s) != 0;
  m->bMirror = getenv("GLED_XR_DESKTOP") == 0;
}

PupilXr::~PupilXr()
{
  Fl::remove_idle(IdleCallback, this);
  m->fPupil->make_current();
  if (m->fDrawable) glXSwapIntervalEXT(fl_display, m->fDrawable, 1);
  m->fPanel.destroy();
  m->fMirror.destroy();
  for (auto &e : m->fEyes)
  {
    if (e.fFBO)   glDeleteFramebuffers(1, &e.fFBO);
    if (e.fDepth) glDeleteRenderbuffers(1, &e.fDepth);
    if (e.fSwapchain) xrDestroySwapchain(e.fSwapchain);
  }
  for (auto s : m->fHandSpace) if (s) xrDestroySpace(s);
  if (m->fAimSpace)  xrDestroySpace(m->fAimSpace);
  if (m->fActionSet) xrDestroyActionSet(m->fActionSet);
  if (m->fSpace)     xrDestroySpace(m->fSpace);
  if (m->fSession)   xrDestroySession(m->fSession);
  if (m->fInstance)  xrDestroyInstance(m->fInstance);
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

  // The desktop window's vsync would hold the headset to the monitor's rate:
  // the next GL call after a swap, a headset render, waits for the blank.
  if (GLXEW_EXT_swap_control)
  {
    glXSwapIntervalEXT(dpy, drw, 0);
    M.fDrawable = drw;
  }

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

  // Controllers: grip, squeeze and stick on both hands; menu and X on the
  // left; aim, trigger, A and B on the right.
  xrStringToPath(M.fInstance, "/user/hand/left",  &M.fHand[0]);
  xrStringToPath(M.fInstance, "/user/hand/right", &M.fHand[1]);
  XrActionSetCreateInfo asci { XR_TYPE_ACTION_SET_CREATE_INFO };
  strcpy(asci.actionSetName, "gled");
  strcpy(asci.localizedActionSetName, "Gled");
  if ( ! M.ok(xrCreateActionSet(M.fInstance, &asci, &M.fActionSet), "xrCreateActionSet")) return false;
  if ( ! M.make_action(XR_ACTION_TYPE_POSE_INPUT,       "grip",     "Grip pose", &M.fGripPose) ||
       ! M.make_action(XR_ACTION_TYPE_FLOAT_INPUT,      "grab",     "Grab",      &M.fGrab)     ||
       ! M.make_action(XR_ACTION_TYPE_VECTOR2F_INPUT,   "stick",    "Stick",     &M.fStick)    ||
       ! M.make_action(XR_ACTION_TYPE_POSE_INPUT,       "aim",      "Aim pose",  &M.fAimPose)  ||
       ! M.make_action(XR_ACTION_TYPE_FLOAT_INPUT,      "trigger",  "Pick",      &M.fTrigger)  ||
       ! M.make_action(XR_ACTION_TYPE_BOOLEAN_INPUT,    "shoot",    "Shoot",     &M.fShoot)    ||
       ! M.make_action(XR_ACTION_TYPE_BOOLEAN_INPUT,    "recenter", "Recenter",  &M.fRecenter) ||
       ! M.make_action(XR_ACTION_TYPE_BOOLEAN_INPUT,    "exit",     "Exit",      &M.fExitAct)  ||
       ! M.make_action(XR_ACTION_TYPE_BOOLEAN_INPUT,    "panel",    "Panel",     &M.fPanelAct) ||
       ! M.make_action(XR_ACTION_TYPE_VIBRATION_OUTPUT, "buzz",     "Buzz",      &M.fBuzz))
    return false;
  M.suggest("/interaction_profiles/oculus/touch_controller",
            { { M.fGripPose, "/user/hand/left/input/grip/pose"      }, { M.fGripPose, "/user/hand/right/input/grip/pose"      },
              { M.fGrab,     "/user/hand/left/input/squeeze/value"  }, { M.fGrab,     "/user/hand/right/input/squeeze/value"  },
              { M.fStick,    "/user/hand/left/input/thumbstick"     }, { M.fStick,    "/user/hand/right/input/thumbstick"     },
              { M.fAimPose,  "/user/hand/right/input/aim/pose"      }, { M.fTrigger,  "/user/hand/right/input/trigger/value"  },
              { M.fShoot,    "/user/hand/right/input/b/click"       }, { M.fRecenter, "/user/hand/right/input/a/click"        },
              { M.fExitAct,  "/user/hand/left/input/menu/click"     }, { M.fPanelAct, "/user/hand/left/input/x/click"         },
              { M.fBuzz,     "/user/hand/left/output/haptic"        }, { M.fBuzz,     "/user/hand/right/output/haptic"        } });
  M.suggest("/interaction_profiles/khr/simple_controller",
            { { M.fGripPose, "/user/hand/left/input/grip/pose"      }, { M.fGripPose, "/user/hand/right/input/grip/pose"      },
              { M.fGrab,     "/user/hand/left/input/select/click"   }, { M.fGrab,     "/user/hand/right/input/select/click"   },
              { M.fExitAct,  "/user/hand/left/input/menu/click"     },
              { M.fBuzz,     "/user/hand/left/output/haptic"        }, { M.fBuzz,     "/user/hand/right/output/haptic"        } });
  if ( ! M.make_space(M.fGripPose, 0, &M.fHandSpace[0]) ||
       ! M.make_space(M.fGripPose, 1, &M.fHandSpace[1]) ||
       ! M.make_space(M.fAimPose,  1, &M.fAimSpace))
    return false;
  XrSessionActionSetsAttachInfo sasai { XR_TYPE_SESSION_ACTION_SETS_ATTACH_INFO };
  sasai.countActionSets = 1;
  sasai.actionSets      = &M.fActionSet;
  if ( ! M.ok(xrAttachSessionActionSets(M.fSession, &sasai), "xrAttachSessionActionSets")) return false;

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
         M.fEyes[0].fW, M.fEyes[0].fH, (unsigned long long) fmt, M.fRig.Scale());
  fflush(stdout);
  return true;
}

bool PupilXr::RequestExit()
{
  if ( ! m->bRunning) return false;
  return XR_SUCCEEDED(xrRequestExitSession(m->fSession));
}

bool PupilXr::Mirroring() const
{
  return m->bMirror;
}

void PupilXr::DrawMirror(int w, int h)
{
  if (m->fMirror.fFBO == 0)
  {
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    return;
  }
  glBindFramebuffer(GL_READ_FRAMEBUFFER, m->fMirror.fFBO);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
  glBlitFramebuffer(0, 0, m->fMirror.fW, m->fMirror.fH, 0, 0, w, h, GL_COLOR_BUFFER_BIT, GL_LINEAR);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
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

void PupilXr::read_input(Input& in, long long t)
{
  Impl &M = *m;

  XrActiveActionSet aas { M.fActionSet, XR_NULL_PATH };
  XrActionsSyncInfo si { XR_TYPE_ACTIONS_SYNC_INFO };
  si.countActiveActionSets = 1;
  si.activeActionSets      = &aas;
  if (xrSyncActions(M.fSession, &si) != XR_SUCCESS)
  {
    M.bGrabPrev[0] = M.bGrabPrev[1] = M.bTriggerPrev = false;
    return;
  }
  in.bFocused = true;

  in.bExit        = M.pressed(M.fExitAct,  0);
  in.bPanelToggle = M.pressed(M.fPanelAct, 0);

  // A: a press shorter than 0.6 s on release, or a long one when it reaches 0.6 s.
  {
    XrActionStateGetInfo gi = M.get_info(M.fRecenter, 1);
    XrActionStateBoolean a { XR_TYPE_ACTION_STATE_BOOLEAN };
    xrGetActionStateBoolean(M.fSession, &gi, &a);
    const bool down = a.isActive && a.currentState;
    if (down && a.changedSinceLastSync) M.fADown = t;
    if (down && M.fADown && t - M.fADown > 600000000) { in.bHome = true; M.fADown = 0; }
    if ( ! down && a.changedSinceLastSync && M.fADown) { in.bRecenter = true; M.fADown = 0; }
  }

  for (int h = 0; h < 2; ++h)
  {
    in.bGripValid[h] = M.locate(M.fHandSpace[h], t, in.fGrip[h]);
    const bool grab = M.held(M.fGrab, h, M.bGrabPrev[h]);
    if (grab && ! M.bGrabPrev[h]) M.buzz(h);
    M.bGrabPrev[h] = grab;
    in.bGrab[h]    = grab && in.bGripValid[h];

    XrActionStateGetInfo gi = M.get_info(M.fStick, h);
    XrActionStateVector2f st { XR_TYPE_ACTION_STATE_VECTOR2F };
    xrGetActionStateVector2f(M.fSession, &gi, &st);
    if (st.isActive)
    {
      in.fStick[2*h]     = st.currentState.x;
      in.fStick[2*h + 1] = st.currentState.y;
    }
  }

  in.bAimValid       = M.locate(M.fAimSpace, t, in.fAim);
  in.bTrigger        = M.held(M.fTrigger, 1, M.bTriggerPrev);
  in.bTriggerChanged = in.bTrigger != M.bTriggerPrev;
  M.bTriggerPrev     = in.bTrigger;

  XrActionStateGetInfo gi = M.get_info(M.fShoot, 1);
  XrActionStateBoolean shoot { XR_TYPE_ACTION_STATE_BOOLEAN };
  xrGetActionStateBoolean(M.fSession, &gi, &shoot);
  in.bShoot        = shoot.isActive && shoot.currentState;
  in.bShootChanged = shoot.isActive && shoot.changedSinceLastSync;
}

void PupilXr::act(const Input& in, double dt)
{
  Impl  &M = *m;
  Pupil &P = *M.fPupil;

  if ( ! in.bFocused)
  {
    // Let go of everything.
    const ZTrans* none[2] = { 0, 0 };
    M.fRig.Grab(none);
    sticks_off();
    M.bAimValid = M.bPanelShown = false;
    return;
  }

  if (in.bExit)        xrRequestExitSession(M.fSession);
  // A held recenters. A short press recenters when a spirit moves the camera,
  // and otherwise only realigns, keeping where flying and grabbing put the scene.
  const bool spirit = P.mEventHandlerImg && P.bUseEventHandler;
  if (in.bHome || (in.bRecenter && spirit)) M.fRig.Recenter();
  else if (in.bRecenter)                    M.fRig.Realign();
  if (in.bPanelToggle) M.bPanelOn = ! M.bPanelOn;

  const ZTrans* hands[2] = { in.bGrab[0] ? &in.fGrip[0] : 0, in.bGrab[1] ? &in.fGrip[1] : 0 };
  const bool holding = M.fRig.Grab(hands);

  if (spirit)
  {
    // The pupil's spirit reads the sticks and moves the camera, which the headset rides.
    PupilInfo &PI = *P.mInfo;
    for (int i = 0; i < 4; ++i) PI.fXrSticks[i] = in.fStick[i];
    PI.fXrSticksOn = true;
  }
  else if ( ! holding)
  {
    M.fRig.Fly(M.fHead, in.fStick, dt);
  }

  if ( ! M.bViewsOk) return;
  M.fRig.Follow(M.fLook, M.fHead);

  // The overlay panel, its lower edge 8 cm above the left hand, facing the
  // head, at 0.5 mm per window pixel and at most 40 cm wide.
  M.bPanelShown = M.bPanelOn && P.mOverlayImg && P.bShowOverlay && in.bGripValid[0];
  if (M.bPanelShown)
  {
    const Int_t bw = M.fBox[2] - M.fBox[0], bh = M.fBox[3] - M.fBox[1];
    const Double_t mpp = TMath::Min(0.0005, 0.4 / TMath::Max(bw, 1));
    M.fPanelHW = 0.5 * mpp * bw;
    M.fPanelHH = 0.5 * mpp * bh;
    ZPoint c = pos_of(in.fGrip[0]);
    c.y += 0.08 + M.fPanelHH;
    ZPoint z = pos_of(M.fHead);
    z -= c;
    if (z.Mag2() < 1e-6) z.Set(0, 0, 1);
    z.Normalize();
    ZPoint x = ZPoint(0, 1, 0).Cross(z);
    if (x.Mag2() < 1e-6) x.Set(1, 0, 0);
    x.Normalize();
    ZPoint y = z.Cross(x);
    M.fPanelFrame.SetBaseVec(1, x);
    M.fPanelFrame.SetBaseVec(2, y);
    M.fPanelFrame.SetBaseVec(3, z);
    M.fPanelFrame.SetBaseVec(4, c);
  }

  // The laser: on the panel it works the overlay; elsewhere it picks and shoots.
  M.bAimValid = in.bAimValid;
  M.fAim      = in.fAim;
  M.fLaserLen = 30;
  bool  hit = false;
  Int_t px = 0, py = 0;
  if (M.bPanelShown && M.fPanelHW > 0 && M.bAimValid)
  {
    const ZPoint o = pos_of(M.fAim), d = axis_of(M.fAim, 2) * -1.0;
    const ZPoint n = axis_of(M.fPanelFrame, 2), c = pos_of(M.fPanelFrame);
    const Double_t dn = d.Dot(n);
    if (dn < -1e-6)
    {
      ZPoint oc(c);
      oc -= o;
      const Double_t t = oc.Dot(n) / dn;
      ZPoint q = d * t;
      q += o;
      q -= c;
      const Double_t lx = q.Dot(axis_of(M.fPanelFrame, 0)), ly = q.Dot(axis_of(M.fPanelFrame, 1));
      if (t > 0 && std::abs(lx) <= M.fPanelHW && std::abs(ly) <= M.fPanelHH)
      {
        hit         = true;
        M.fLaserLen = t;
        px = M.fBox[0] + (Int_t) ((lx + M.fPanelHW) / (2 * M.fPanelHW) * (M.fBox[2] - M.fBox[0]));
        py = P.h() - (M.fBox[1] + (Int_t) ((ly + M.fPanelHH) / (2 * M.fPanelHH) * (M.fBox[3] - M.fBox[1])));
      }
    }
  }

  const bool moved = hit && (! M.bPanelHit || px != M.fPanelX || py != M.fPanelY);
  if (M.bPanelPress)
  {
    if (moved) overlay_event(FL_DRAG, px, py);
  }
  else if (moved)
  {
    overlay_event(FL_MOVE, px, py);
  }
  else if ( ! hit && M.bPanelHit)
  {
    overlay_event(FL_LEAVE, M.fPanelX, M.fPanelY);
  }
  if (hit) { M.fPanelX = px; M.fPanelY = py; }
  M.bPanelHit = hit;

  if (in.bTriggerChanged)
  {
    if ( ! in.bTrigger)
    {
      if (M.bPanelPress) overlay_event(FL_RELEASE, M.fPanelX, M.fPanelY);
      if (M.bScenePress) laser_event(FL_RELEASE, 1);
      M.bPanelPress = M.bScenePress = false;
    }
    else if (hit)
    {
      overlay_event(FL_PUSH, px, py);
      M.bPanelPress = true;
    }
    else
    {
      laser_event(FL_PUSH, 1);
      M.bScenePress = true;
    }
  }
  if (in.bShootChanged && ! hit)
    laser_event(in.bShoot ? FL_PUSH : FL_RELEASE, 3);
}

void PupilXr::sticks_off()
{
  PupilInfo &PI = *m->fPupil->mInfo;
  PI.fXrSticksOn = false;
  for (auto &s : PI.fXrSticks) s = 0;
}

void PupilXr::laser_event(int ev, int button)
{
  // A mouse event along the laser, with the ray in the camera-base frame,
  // through the pupil's normal event delivery.
  Impl  &M = *m;
  Pupil &P = *M.fPupil;
  if ( ! M.bAimValid) return;

  ZTrans inv = M.fRig.RefFromWorld() * M.fBase;
  inv.Invert();
  ZPoint pos = pos_of(M.fAim), dir = axis_of(M.fAim, 2) * -1.0;
  inv.MultiplyIP(pos);
  inv.RotateIP(dir);
  dir.Normalize();

  A_Rnr::Fl_Event e;
  P.setup_rnr_event(ev, e);
  e.fButton  = button;
  e.fButtons = ev == FL_PUSH ? FL_BUTTON(button) : 0;
  e.fState   = e.fButtons;
  P.mDriver->SetFixedMouseRay(pos, dir);
  P.deliver_rnr_event(e);
  P.mDriver->ReleaseFixedMouseRay();
}

void PupilXr::overlay_event(int ev, int x, int y)
{
  // A mouse event at window pixels of the panel, to the overlay only. Overlay
  // picking uses the window's viewport.
  Pupil &P = *m->fPupil;
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glViewport(0, 0, P.w(), P.h());

  A_Rnr::Fl_Event e;
  P.setup_rnr_event(ev, e);
  e.fX         = x;
  e.fY         = y;
  e.fButton    = 1;
  e.fButtons   = (ev == FL_PUSH || ev == FL_DRAG) ? FL_BUTTON1 : 0;
  e.fState     = e.fButtons;
  e.fText      = "";
  e.fIsOverlay = true;
  P.mDriver->PreEventHandling(e);
  P.handle_overlay(e);
}

//==============================================================================

void PupilXr::render_panel()
{
  // The overlay alone, as the pupil's window draws it, on a clear background.
  Impl  &M = *m;
  Pupil &P = *M.fPupil;
  if ( ! M.bPanelShown) return;

  M.fPanel.ensure(P.w(), P.h(), true);
  glBindFramebuffer(GL_FRAMEBUFFER, M.fPanel.fFBO);
  glViewport(0, 0, P.w(), P.h());
  // The overlay's projection starts from the pupil's base projection, which an
  // overlay pick leaves as its pick matrix; SetProjection resets it.
  glMatrixMode(GL_PROJECTION); glLoadIdentity();
  P.SetProjection();
  glMatrixMode(GL_MODELVIEW);  glLoadIdentity();
  P.rnr_default_init();
  glClearColor(0, 0, 0, 0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  P.Render(false, true);

  // The box of drawn pixels, with a margin; re-found every 15 frames.
  if (M.fFrames - M.fBoxFrame >= 15)
  {
    const Int_t w = P.w(), h = P.h();
    std::vector<unsigned char> a(w * h);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadPixels(0, 0, w, h, GL_ALPHA, GL_UNSIGNED_BYTE, a.data());
    Int_t x0 = w, y0 = h, x1 = -1, y1 = -1;
    for (Int_t y = 0; y < h; ++y)
      for (Int_t x = 0; x < w; ++x)
        if (a[y * w + x])
        {
          if (x < x0) x0 = x;
          if (x > x1) x1 = x;
          if (y < y0) y0 = y;
          if (y > y1) y1 = y;
        }
    const Int_t mg = 8;
    if (x1 < 0)
    {
      M.fBox[0] = M.fBox[1] = M.fBox[2] = M.fBox[3] = 0;
    }
    else
    {
      M.fBox[0] = TMath::Max(x0 - mg, 0); M.fBox[1] = TMath::Max(y0 - mg, 0);
      M.fBox[2] = TMath::Min(x1 + 1 + mg, w); M.fBox[3] = TMath::Min(y1 + 1 + mg, h);
    }
    M.fBoxFrame = M.fFrames;
  }
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
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

  // Near and far in metres, as the scene can be scaled by hand.
  const XrFovf  &fov = M.fViews[e].fov;
  const Double_t n = 0.05, f = 1000;
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glFrustum(n*std::tan(fov.angleLeft), n*std::tan(fov.angleRight),
            n*std::tan(fov.angleDown), n*std::tan(fov.angleUp), n, f);

  // OpenXR's view space has GL's eye axes: the inverse eye pose, then the
  // scene's placement and the ridden frame's view.
  ZTrans view = pose_to_trans(M.fViews[e].pose);
  view.Invert();
  ZTrans mv = view * M.fRig.RefFromWorld();
  glMatrixMode(GL_MODELVIEW);
  glLoadMatrixd(mv.Array());

  P.rnr_default_init();
  // The placement scales, so normals need renormalising for lighting.
  glEnable(GL_NORMALIZE);
  P.Render(true, false);
  glDisable(GL_NORMALIZE);

  glLoadMatrixd(view.Array());
  glPushAttrib(GL_ENABLE_BIT | GL_LINE_BIT | GL_CURRENT_BIT | GL_COLOR_BUFFER_BIT | GL_TEXTURE_BIT);
  glDisable(GL_LIGHTING);
  if (M.bPanelShown && M.fPanelHW > 0)
  {
    // The panel: a dim backing, then the overlay's box over it.
    glPushMatrix();
    glMultMatrixd(M.fPanelFrame.Array());
    const Double_t hw = M.fPanelHW, hh = M.fPanelHH;
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.1f, 0.1f, 0.15f, 0.35f);
    glBegin(GL_QUADS);
    glVertex3d(-hw, -hh, 0); glVertex3d(hw, -hh, 0); glVertex3d(hw, hh, 0); glVertex3d(-hw, hh, 0);
    glEnd();
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, M.fPanel.fTex);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glColor4f(1, 1, 1, 1);
    const Float_t u0 = (Float_t) M.fBox[0] / P.w(), u1 = (Float_t) M.fBox[2] / P.w();
    const Float_t v0 = (Float_t) M.fBox[1] / P.h(), v1 = (Float_t) M.fBox[3] / P.h();
    glBegin(GL_QUADS);
    glTexCoord2f(u0, v0); glVertex3d(-hw, -hh, 0.001);
    glTexCoord2f(u1, v0); glVertex3d( hw, -hh, 0.001);
    glTexCoord2f(u1, v1); glVertex3d( hw,  hh, 0.001);
    glTexCoord2f(u0, v1); glVertex3d(-hw,  hh, 0.001);
    glEnd();
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
    glPopMatrix();
  }
  if (M.bAimValid)
  {
    // The laser from the right hand, to the panel when it is on it.
    const ZPoint a = pos_of(M.fAim), d = axis_of(M.fAim, 2) * -M.fLaserLen;
    glLineWidth(3);
    if (M.bTriggerPrev) glColor3f(1, 1, 0.3f); else glColor3f(1, 0.25f, 0.1f);
    glBegin(GL_LINES);
    glVertex3d(a.x, a.y, a.z);
    glVertex3d(a.x + d.x, a.y + d.y, a.z + d.z);
    glEnd();
  }
  glPopAttrib();

  if (e == 0 && M.bMirror)
  {
    // The middle of the left eye, at the window's aspect, for the desktop.
    const Int_t w = P.w(), h = P.h();
    M.fMirror.ensure(w, h, false);
    Int_t sw = E.fW, sh = E.fH;
    if ((Double_t) w / h > (Double_t) sw / sh) sh = sw * h / w; else sw = sh * w / h;
    const Int_t sx = (E.fW - sw) / 2, sy = (E.fH - sh) / 2;
    glBindFramebuffer(GL_READ_FRAMEBUFFER, E.fFBO);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, M.fMirror.fFBO);
    glBlitFramebuffer(sx, sy, sx + sw, sy + sh, 0, 0, w, h, GL_COLOR_BUFFER_BIT, GL_LINEAR);
  }

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  XrSwapchainImageReleaseInfo ri { XR_TYPE_SWAPCHAIN_IMAGE_RELEASE_INFO };
  xrReleaseSwapchainImage(E.fSwapchain, &ri);
}

void PupilXr::frame()
{
  Impl  &M = *m;
  Pupil &P = *M.fPupil;

  if ( ! poll_events())
  {
    printf("PupilXr: session ended after %lld frames.\n", M.fFrames);
    fflush(stdout);
    sticks_off();
    P.mXr = 0;
    delete this;
    P.redraw();
    return;
  }
  if ( ! M.bRunning)
  {
    usleep(10000);
    return;
  }

  const GTime t0 = GTime::Now();
  if (M.fFrames > 0) M.fGapSum += (t0 - M.fFrameEnd).ToDouble();

  P.make_current();

  XrFrameWaitInfo wi { XR_TYPE_FRAME_WAIT_INFO };
  XrFrameState    fs { XR_TYPE_FRAME_STATE };
  if ( ! M.ok(xrWaitFrame(M.fSession, &wi, &fs), "xrWaitFrame")) return;
  const GTime t1 = GTime::Now();
  M.fWaitSum += (t1 - t0).ToDouble();
  M.fPeriod   = fs.predictedDisplayPeriod;
  XrFrameBeginInfo bi { XR_TYPE_FRAME_BEGIN_INFO };
  if ( ! M.ok(xrBeginFrame(M.fSession, &bi), "xrBeginFrame")) return;

  const XrTime   t  = fs.predictedDisplayTime;
  const Double_t dt = M.fLastTime ? TMath::Min(1e-9 * (t - M.fLastTime), 0.1) : 0;
  M.fLastTime = t;

  // The eyes, the head between them, and the frame the headset rides.
  M.bViewsOk = false;
  if (fs.shouldRender)
  {
    XrViewLocateInfo li { XR_TYPE_VIEW_LOCATE_INFO };
    li.viewConfigurationType = XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO;
    li.displayTime           = t;
    li.space                 = M.fSpace;
    XrViewState vs { XR_TYPE_VIEW_STATE };
    uint32_t    n_views = 0;
    M.ok(xrLocateViews(M.fSession, &li, &vs, 2, &n_views, M.fViews), "xrLocateViews");
    const XrViewStateFlags valid = XR_VIEW_STATE_POSITION_VALID_BIT | XR_VIEW_STATE_ORIENTATION_VALID_BIT;
    if (n_views == 2 && (vs.viewStateFlags & valid) == valid)
    {
      M.fHead = pose_to_trans(M.fViews[0].pose);
      const XrVector3f &a = M.fViews[0].pose.position, &b = M.fViews[1].pose.position;
      M.fHead.CM(0, 3) = (a.x + b.x) / 2;
      M.fHead.CM(1, 3) = (a.y + b.y) / 2;
      M.fHead.CM(2, 3) = (a.z + b.z) / 2;
      try
      {
        ZTrans ride = P.RideFrame();
        ride.Invert();
        // Gled's frame (x forward, y left, z up) to GL's eye axes (x right, y up, -z forward).
        ZTrans axes;
        axes.CM(0, 0) = 0; axes.CM(0, 1) = -1;
        axes.CM(1, 1) = 0; axes.CM(1, 2) =  1;
        axes.CM(2, 2) = 0; axes.CM(2, 0) = -1;
        M.fLook  = axes * ride;
        M.fBase  = P.mCamBaseTrans;
        M.bViewsOk = true;
      }
      catch (Exc_t& exc)
      {
        printf("PupilXr: camera setup failed: %s.\n", exc.Data());
      }
    }
  }

  Input in;
  read_input(in, t);
  act(in, dt);

  XrCompositionLayerProjectionView pv[2];
  XrCompositionLayerProjection     layer { XR_TYPE_COMPOSITION_LAYER_PROJECTION };
  const XrCompositionLayerBaseHeader *layers[1];
  uint32_t n_layers = 0;
  if (M.bViewsOk)
  {
    // Every xrBeginFrame needs its xrEndFrame.
    try { render_panel(); }
    catch (Exc_t& exc) { printf("PupilXr: panel render failed: %s.\n", exc.Data()); }
    for (int e = 0; e < 2; ++e)
    {
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

  XrFrameEndInfo ei { XR_TYPE_FRAME_END_INFO };
  ei.displayTime          = t;
  ei.environmentBlendMode = XR_ENVIRONMENT_BLEND_MODE_OPAQUE;
  ei.layerCount           = n_layers;
  ei.layers               = layers;
  M.ok(xrEndFrame(M.fSession, &ei), "xrEndFrame");
  if (M.bMirror) P.redraw();

  M.fFrameEnd = GTime::Now();
  M.fWorkSum += (M.fFrameEnd - t1).ToDouble();
  if (M.fFrames++ % 600 == 0)
  {
    if (M.fFrames > 1)
      printf("PupilXr: %.1f frames per second, display period %.2f ms; per frame: "
             "wait %.2f ms, render %.2f ms, between frames %.2f ms.\n",
             600 / (M.fFrameEnd - M.fRateStart).ToDouble(), 1e-6 * M.fPeriod,
             1e3 * M.fWaitSum / 600, 1e3 * M.fWorkSum / 600, 1e3 * M.fGapSum / 600);
    fflush(stdout);
    M.fRateStart = M.fFrameEnd;
    M.fWaitSum   = M.fWorkSum = M.fGapSum = 0;
  }
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

bool PupilXr::RequestExit()          { return false; }
bool PupilXr::Mirroring() const      { return false; }
void PupilXr::DrawMirror(int, int)   {}
void PupilXr::IdleCallback(void*)    {}

#endif
