// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "PupilXr.h"
#include "Pupil.h"
#include <Glasses/PupilInfo.h>
#include <Stones/ZTrans.h>
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

  // Translation, uniform scale, and a turn about the vertical (+y).
  ZTrans translation(Double_t x, Double_t y, Double_t z)
  { ZTrans t; t.CM(0, 3) = x; t.CM(1, 3) = y; t.CM(2, 3) = z; return t; }

  ZTrans scaling(Double_t s)
  { ZTrans t; for (int i = 0; i < 3; ++i) t.CM(i, i) = s; return t; }

  // The turn of a rotation about the vertical, stable however the hand is held.
  Double_t yaw_of(const ZTrans& t)
  { return std::atan2(t.CM(0, 2) - t.CM(2, 0), t.CM(0, 0) + t.CM(2, 2)); }

  ZTrans yaw(Double_t a)
  {
    ZTrans t;
    t.CM(0, 0) =  std::cos(a); t.CM(0, 2) = std::sin(a);
    t.CM(2, 0) = -std::sin(a); t.CM(2, 2) = std::cos(a);
    return t;
  }
}

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
  Long64_t        fFrames   = 0;
  GTime           fRateStart;

  // Controllers, left and right.
  XrActionSet     fActionSet = XR_NULL_HANDLE;
  XrAction        fPose = XR_NULL_HANDLE, fGrab = XR_NULL_HANDLE,
                  fMenu = XR_NULL_HANDLE, fBuzz = XR_NULL_HANDLE,
                  fReset = XR_NULL_HANDLE, fStick = XR_NULL_HANDLE,
                  fAimPose = XR_NULL_HANDLE, fSelect = XR_NULL_HANDLE, fShoot = XR_NULL_HANDLE;
  XrPath          fHand[2]      = { XR_NULL_PATH, XR_NULL_PATH };
  XrSpace         fHandSpace[2] = { XR_NULL_HANDLE, XR_NULL_HANDLE };
  bool            bHeld[2]      = { false, false };

  // The camera's view frame, in gled units, to the reference space, in metres.
  ZTrans          fW, fWStart;
  // Grab anchors: the hands that hold, and fW and where the hands were when they took hold.
  int             fGrabMask = 0;
  ZTrans          fW0;
  ZPoint          fMid0;
  Double_t        fDist0 = 1, fYaw0 = 0;
  // Flying along the gaze: speed in metres per second, acceleration of the stick.
  Double_t        fSpeed = 0, fFlyAccel = 3;
  XrTime          fLastTime = 0;
  // The right hand's laser, in the reference space, and its trigger and B.
  XrSpace         fAimSpace = XR_NULL_HANDLE;
  ZTrans          fAim;
  bool            bAimValid = false, bSelectDown = false;
  // The camera's view and the camera base of the last frame, for the laser's ray.
  ZTrans          fLook, fBase;

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

  void buzz(int h)
  {
    XrHapticVibration v { XR_TYPE_HAPTIC_VIBRATION };
    v.amplitude = 0.4f;
    v.duration  = 15000000; // ns
    v.frequency = XR_FREQUENCY_UNSPECIFIED;
    XrHapticActionInfo hi { XR_TYPE_HAPTIC_ACTION_INFO };
    hi.action        = fBuzz;
    hi.subactionPath = fHand[h];
    xrApplyHapticFeedback(fSession, &hi, (XrHapticBaseHeader*) &v);
  }
};

//==============================================================================

PupilXr::PupilXr(Pupil* pupil) : m(new Impl)
{
  m->fPupil = pupil;
  for (auto &v : m->fViews) v = { XR_TYPE_VIEW };
  Double_t scale = 0.2;  // Metres per gled unit.
  if (const char* s = getenv("GLED_XR_SCALE"))
    scale = atof(s);
  m->fW = m->fWStart = scaling(scale);
  if (const char* s = getenv("GLED_XR_FLY"))
    m->fFlyAccel = atof(s);
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
  for (auto s : m->fHandSpace) if (s) xrDestroySpace(s);
  if (m->fAimSpace) xrDestroySpace(m->fAimSpace);
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

  // Controllers: grip, squeeze and stick on both hands; menu on the left;
  // aim, trigger, A and B on the right.
  xrStringToPath(M.fInstance, "/user/hand/left",  &M.fHand[0]);
  xrStringToPath(M.fInstance, "/user/hand/right", &M.fHand[1]);
  XrActionSetCreateInfo asci { XR_TYPE_ACTION_SET_CREATE_INFO };
  strcpy(asci.actionSetName, "gled");
  strcpy(asci.localizedActionSetName, "Gled");
  if ( ! M.ok(xrCreateActionSet(M.fInstance, &asci, &M.fActionSet), "xrCreateActionSet")) return false;
  if ( ! M.make_action(XR_ACTION_TYPE_POSE_INPUT,       "grip",  "Grip pose", &M.fPose) ||
       ! M.make_action(XR_ACTION_TYPE_FLOAT_INPUT,      "grab",  "Grab",      &M.fGrab) ||
       ! M.make_action(XR_ACTION_TYPE_BOOLEAN_INPUT,    "exit",  "Exit",      &M.fMenu) ||
       ! M.make_action(XR_ACTION_TYPE_VIBRATION_OUTPUT, "buzz",  "Buzz",      &M.fBuzz) ||
       ! M.make_action(XR_ACTION_TYPE_BOOLEAN_INPUT,    "reset", "Reset",     &M.fReset) ||
       ! M.make_action(XR_ACTION_TYPE_VECTOR2F_INPUT,   "stick", "Stick",     &M.fStick) ||
       ! M.make_action(XR_ACTION_TYPE_POSE_INPUT,       "aim",   "Aim pose",  &M.fAimPose) ||
       ! M.make_action(XR_ACTION_TYPE_FLOAT_INPUT,      "select", "Select",   &M.fSelect) ||
       ! M.make_action(XR_ACTION_TYPE_BOOLEAN_INPUT,    "shoot", "Shoot",     &M.fShoot))
    return false;
  M.suggest("/interaction_profiles/oculus/touch_controller",
            { { M.fPose, "/user/hand/left/input/grip/pose"     }, { M.fPose, "/user/hand/right/input/grip/pose"     },
              { M.fGrab, "/user/hand/left/input/squeeze/value" }, { M.fGrab, "/user/hand/right/input/squeeze/value" },
              { M.fMenu, "/user/hand/left/input/menu/click"    }, { M.fReset, "/user/hand/right/input/a/click"      },
              { M.fStick, "/user/hand/left/input/thumbstick"   }, { M.fStick, "/user/hand/right/input/thumbstick"   },
              { M.fAimPose, "/user/hand/right/input/aim/pose"  }, { M.fSelect, "/user/hand/right/input/trigger/value" },
              { M.fShoot, "/user/hand/right/input/b/click"     },
              { M.fBuzz, "/user/hand/left/output/haptic"       }, { M.fBuzz, "/user/hand/right/output/haptic"       } });
  M.suggest("/interaction_profiles/khr/simple_controller",
            { { M.fPose, "/user/hand/left/input/grip/pose"     }, { M.fPose, "/user/hand/right/input/grip/pose"     },
              { M.fGrab, "/user/hand/left/input/select/click"  }, { M.fGrab, "/user/hand/right/input/select/click"  },
              { M.fMenu, "/user/hand/left/input/menu/click"    },
              { M.fBuzz, "/user/hand/left/output/haptic"       }, { M.fBuzz, "/user/hand/right/output/haptic"       } });
  for (int h = 0; h < 2; ++h)
  {
    XrActionSpaceCreateInfo ascr { XR_TYPE_ACTION_SPACE_CREATE_INFO };
    ascr.action        = M.fPose;
    ascr.subactionPath = M.fHand[h];
    ascr.poseInActionSpace.orientation.w = 1;
    if ( ! M.ok(xrCreateActionSpace(M.fSession, &ascr, &M.fHandSpace[h]), "xrCreateActionSpace")) return false;
  }
  {
    XrActionSpaceCreateInfo ascr { XR_TYPE_ACTION_SPACE_CREATE_INFO };
    ascr.action        = M.fAimPose;
    ascr.subactionPath = M.fHand[1];
    ascr.poseInActionSpace.orientation.w = 1;
    if ( ! M.ok(xrCreateActionSpace(M.fSession, &ascr, &M.fAimSpace), "xrCreateActionSpace")) return false;
  }
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
         M.fEyes[0].fW, M.fEyes[0].fH, (unsigned long long) fmt, M.fW.CM(0, 0));
  fflush(stdout);
  return true;
}

bool PupilXr::RequestExit()
{
  if ( ! m->bRunning) return false;
  return XR_SUCCEEDED(xrRequestExitSession(m->fSession));
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

void PupilXr::sticks_off()
{
  PupilInfo &PI = *m->fPupil->mInfo;
  PI.fXrSticksOn = false;
  for (auto &s : PI.fXrSticks) s = 0;
}

void PupilXr::laser_event(int ev, int button)
{
  // A mouse event along the laser, with the ray in the camera-base frame.
  Impl  &M = *m;
  Pupil &P = *M.fPupil;
  if ( ! M.bAimValid) return;

  ZTrans inv = M.fW * M.fLook * M.fBase;
  inv.Invert();
  ZPoint pos(M.fAim.CM(0, 3), M.fAim.CM(1, 3), M.fAim.CM(2, 3));
  ZPoint dir(-M.fAim.CM(0, 2), -M.fAim.CM(1, 2), -M.fAim.CM(2, 2));
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

void PupilXr::update_input(long long display_time)
{
  Impl &M = *m;

  XrActiveActionSet aas { M.fActionSet, XR_NULL_PATH };
  XrActionsSyncInfo si { XR_TYPE_ACTIONS_SYNC_INFO };
  si.countActiveActionSets = 1;
  si.activeActionSets      = &aas;
  if (xrSyncActions(M.fSession, &si) != XR_SUCCESS)
  {
    // Not focused: let go.
    M.fGrabMask = 0;
    M.bHeld[0]  = M.bHeld[1] = false;
    sticks_off();
    return;
  }

  XrActionStateGetInfo gi { XR_TYPE_ACTION_STATE_GET_INFO };
  gi.action        = M.fMenu;
  gi.subactionPath = M.fHand[0];
  XrActionStateBoolean menu { XR_TYPE_ACTION_STATE_BOOLEAN };
  xrGetActionStateBoolean(M.fSession, &gi, &menu);
  if (menu.isActive && menu.changedSinceLastSync && menu.currentState)
    xrRequestExitSession(M.fSession);

  gi.action        = M.fReset;
  gi.subactionPath = M.fHand[1];
  XrActionStateBoolean reset { XR_TYPE_ACTION_STATE_BOOLEAN };
  xrGetActionStateBoolean(M.fSession, &gi, &reset);
  if (reset.isActive && reset.changedSinceLastSync && reset.currentState)
  {
    M.fW        = M.fWStart;
    M.fSpeed    = 0;
    M.fGrabMask = -1; // Re-anchor any hand still holding.
  }

  const Double_t dt = M.fLastTime ? TMath::Min(1e-9 * (display_time - M.fLastTime), 0.1) : 0;
  M.fLastTime = display_time;

  ZTrans H[2];
  int    mask = 0;
  for (int h = 0; h < 2; ++h)
  {
    gi.action        = M.fGrab;
    gi.subactionPath = M.fHand[h];
    XrActionStateFloat grab { XR_TYPE_ACTION_STATE_FLOAT };
    xrGetActionStateFloat(M.fSession, &gi, &grab);
    // Hysteresis, so a half-pressed grip does not flicker.
    bool held = grab.isActive && grab.currentState > (M.bHeld[h] ? 0.4f : 0.6f);
    if (held && ! M.bHeld[h]) M.buzz(h);
    M.bHeld[h] = held;

    XrSpaceLocation loc { XR_TYPE_SPACE_LOCATION };
    xrLocateSpace(M.fHandSpace[h], M.fSpace, display_time, &loc);
    const XrSpaceLocationFlags valid = XR_SPACE_LOCATION_POSITION_VALID_BIT | XR_SPACE_LOCATION_ORIENTATION_VALID_BIT;
    if (held && (loc.locationFlags & valid) == valid)
    {
      H[h]  = pose_to_trans(loc.pose);
      mask |= 1 << h;
    }
  }

  ZPoint   pl(H[0].CM(0, 3), H[0].CM(1, 3), H[0].CM(2, 3));
  ZPoint   pr(H[1].CM(0, 3), H[1].CM(1, 3), H[1].CM(2, 3));
  ZPoint   mid((pl.x + pr.x) / 2, (pl.y + pr.y) / 2, (pl.z + pr.z) / 2);
  Double_t dist = pl.Distance(pr);
  Double_t ang  = std::atan2(pl.x - pr.x, pl.z - pr.z);

  XrVector2f stick[2] = { { 0, 0 }, { 0, 0 } };
  for (int h = 0; h < 2; ++h)
  {
    gi.action        = M.fStick;
    gi.subactionPath = M.fHand[h];
    XrActionStateVector2f st { XR_TYPE_ACTION_STATE_VECTOR2F };
    xrGetActionStateVector2f(M.fSession, &gi, &st);
    if (st.isActive) stick[h] = st.currentState;
  }

  // The laser: aim pose, trigger to pick, B to shoot, as mouse buttons 1 and 3.
  {
    XrSpaceLocation loc { XR_TYPE_SPACE_LOCATION };
    xrLocateSpace(M.fAimSpace, M.fSpace, display_time, &loc);
    const XrSpaceLocationFlags valid = XR_SPACE_LOCATION_POSITION_VALID_BIT | XR_SPACE_LOCATION_ORIENTATION_VALID_BIT;
    M.bAimValid = (loc.locationFlags & valid) == valid;
    if (M.bAimValid) M.fAim = pose_to_trans(loc.pose);

    gi.action        = M.fSelect;
    gi.subactionPath = M.fHand[1];
    XrActionStateFloat sel { XR_TYPE_ACTION_STATE_FLOAT };
    xrGetActionStateFloat(M.fSession, &gi, &sel);
    const bool sel_down = sel.isActive && sel.currentState > (M.bSelectDown ? 0.4f : 0.6f);
    if (sel_down != M.bSelectDown)
    {
      M.bSelectDown = sel_down;
      laser_event(sel_down ? FL_PUSH : FL_RELEASE, 1);
    }

    gi.action        = M.fShoot;
    XrActionStateBoolean shoot { XR_TYPE_ACTION_STATE_BOOLEAN };
    xrGetActionStateBoolean(M.fSession, &gi, &shoot);
    if (shoot.isActive && shoot.changedSinceLastSync)
      laser_event(shoot.currentState ? FL_PUSH : FL_RELEASE, 3);
  }

  Pupil &P = *M.fPupil;
  if (P.mEventHandlerImg && P.bUseEventHandler)
  {
    // The pupil's spirit reads the sticks and moves the camera, which the headset rides.
    PupilInfo &PI = *P.mInfo;
    PI.fXrSticks[0] = stick[0].x; PI.fXrSticks[1] = stick[0].y;
    PI.fXrSticks[2] = stick[1].x; PI.fXrSticks[3] = stick[1].y;
    PI.fXrSticksOn  = true;
  }
  else if (mask == 0)
  {
    // Left stick: speed up along the gaze, brake and reverse, coast to a stop
    // when let go; and strafe. Right stick: turn, and like a plane, push to go down.
    if (std::abs(stick[0].y) > 0.15)
      M.fSpeed = TMath::Range(-50.0, 50.0, M.fSpeed + stick[0].y * M.fFlyAccel * dt);
    else
      M.fSpeed *= std::exp(-dt / 0.4);

    ZTrans head = pose_to_trans(M.fViews[0].pose);
    const Double_t f = M.fSpeed * dt, s = 2 * stick[0].x * dt, u = 2 * stick[1].y * dt;
    ZTrans move = translation(f * head.CM(0, 2) - s * head.CM(0, 0),
                              f * head.CM(1, 2) - s * head.CM(1, 0) + u,
                              f * head.CM(2, 2) - s * head.CM(2, 0));
    if (std::abs(stick[1].x) > 0.15)
    {
      const Double_t hx = head.CM(0, 3), hy = head.CM(1, 3), hz = head.CM(2, 3);
      move = translation(hx, hy, hz) * yaw(-stick[1].x * dt) * translation(-hx, -hy, -hz) * move;
    }
    M.fW = move * M.fW;
  }
  if (mask != 0)
  {
    M.fSpeed = 0;
  }

  if (mask != M.fGrabMask)
  {
    // A hand took hold or let go: anchor at the current placement.
    M.fW0       = M.fW;
    M.fGrabMask = mask;
    if (mask == 1 || mask == 2)
    {
      const ZTrans &h = H[mask - 1];
      M.fMid0 = ZPoint(h.CM(0, 3), h.CM(1, 3), h.CM(2, 3));
      M.fYaw0 = yaw_of(h);
    }
    else if (mask == 3)
    {
      M.fMid0  = mid;
      M.fDist0 = TMath::Max(dist, 1e-3);
      M.fYaw0  = ang;
    }
    return;
  }

  if (mask == 1 || mask == 2)
  {
    // The scene follows the hand's position and heading, and stays level.
    const ZTrans &h = H[mask - 1];
    M.fW = translation(h.CM(0, 3), h.CM(1, 3), h.CM(2, 3)) *
           yaw(yaw_of(h) - M.fYaw0) *
           translation(-M.fMid0.x, -M.fMid0.y, -M.fMid0.z) * M.fW0;
  }
  else if (mask == 3)
  {
    // Two hands: scale by their distance, turn about the vertical, move with their midpoint.
    M.fW = translation(mid.x, mid.y, mid.z) * yaw(ang - M.fYaw0) * scaling(dist / M.fDist0) *
           translation(-M.fMid0.x, -M.fMid0.y, -M.fMid0.z) * M.fW0;
  }
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
  // scene's placement, then the pupil's camera.
  ZTrans view = pose_to_trans(M.fViews[e].pose);
  view.Invert();
  ZTrans mv = view * M.fW;
  glMatrixMode(GL_MODELVIEW);
  try
  {
    P.SetAbsRelCamera();
  }
  catch (Exc_t& exc)
  {
    printf("PupilXr: camera setup failed: %s.\n", exc.Data());
  }
  // Ride the camera: its position and heading, with the camera base's z as up,
  // so a spirit that keeps the base on a surface keeps the headset level on it.
  const ZTrans &cam = P.mCamAbsTrans, &base = P.mCamBaseTrans;
  ZPoint up(base.CM(0, 2), base.CM(1, 2), base.CM(2, 2));
  up.Normalize();
  ZPoint fwd(cam.CM(0, 0), cam.CM(1, 0), cam.CM(2, 0));
  fwd -= up * fwd.Dot(up);
  if (fwd.Mag2() < 1e-12) fwd = up.Orthogonal();
  fwd.Normalize();
  const Double_t *pos = cam.ArrT();
  glLoadIdentity();
  gluLookAt(pos[0], pos[1], pos[2], pos[0] + fwd.x, pos[1] + fwd.y, pos[2] + fwd.z, up.x, up.y, up.z);
  glGetDoublev(GL_MODELVIEW_MATRIX, M.fLook.Array());
  M.fBase = base;
  glLoadMatrixd(mv.Array());
  glMultMatrixd(M.fLook.Array());

  P.rnr_default_init();
  // The placement scales, so normals need renormalising for lighting.
  glEnable(GL_NORMALIZE);
  P.Render(true, false);
  glDisable(GL_NORMALIZE);

  if (M.bAimValid)
  {
    // The laser, 30 m from the right hand.
    const ZTrans &a = M.fAim;
    glLoadMatrixd(view.Array());
    glPushAttrib(GL_ENABLE_BIT | GL_LINE_BIT | GL_CURRENT_BIT);
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
    glLineWidth(3);
    if (M.bSelectDown) glColor3f(1, 1, 0.3f); else glColor3f(1, 0.25f, 0.1f);
    glBegin(GL_LINES);
    glVertex3d(a.CM(0, 3), a.CM(1, 3), a.CM(2, 3));
    glVertex3d(a.CM(0, 3) - 30*a.CM(0, 2), a.CM(1, 3) - 30*a.CM(1, 2), a.CM(2, 3) - 30*a.CM(2, 2));
    glEnd();
    glPopAttrib();
  }

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
    fflush(stdout);
    sticks_off();
    Pupil *pupil = M.fPupil;
    pupil->mXr = 0;
    delete this;
    pupil->redraw();
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

  update_input(fs.predictedDisplayTime);

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
  if (M.fFrames++ % 600 == 0)
  {
    GTime now = GTime::Now();
    if (M.fFrames > 1)
      printf("PupilXr: %.1f frames per second.\n", 600 / (now - M.fRateStart).ToDouble());
    fflush(stdout);
    M.fRateStart = now;
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

bool PupilXr::RequestExit() { return false; }

void PupilXr::IdleCallback(void* pxr) {}

#endif
