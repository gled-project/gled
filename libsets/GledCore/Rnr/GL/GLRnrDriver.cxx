// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

//______________________________________________________________________
// GLRnrDriver
//

#include "GLRnrDriver.h"
#include <RnrBase/Fl_Event_Enums.h>
#include <Glasses/ZGlColorFader.h>

#include <TMath.h>

#include <algorithm>
#include <bit>
#include <cstdlib>

using namespace gled;


GLRnrDriver::GLRnrDriver(Eye* e, const TString& r) : RnrDriver(e, r)
{
  bInPicking = false;
  bDoPickOps = false;
  mPickCount = 0;
  mPickSize  = 1024;
  mPickVector.resize(mPickSize);

  mBelowMouse = mPushed = mFocus = 0;
  bFixedMouseRay = false;

  const char* stats = getenv("GLED_RNR_STATS");
  mStatsEvery = stats ? atoi(stats) : 0;
  mStatsN = 0;
  mStatsSum = mStatsMax = mStatsCulled = mStatsCulledParts = mStatsBlended = mStatsReplays = 0;

  mMaxLamps = 8;
  mLamps = new A_Rnr* [mMaxLamps];

  mMaxClipPlanes = 6;
  mClipPlanes = new A_Rnr* [mMaxClipPlanes];

  bMarkupNodes = false;

  bRedraw = false;

  mFaderStack = 0;

  // ----------------------------------------------------------------

  mQuadricStd = 0;
  mQuadricStdNoNormals = 0;
}

GLRnrDriver::~GLRnrDriver()
{
  delete [] mLamps;
  delete [] mClipPlanes;

  if (mQuadricStd != 0)
  {
    gluDeleteQuadric(mQuadricStd);
    gluDeleteQuadric(mQuadricStdNoNormals);
  }
}

/**************************************************************************/
// Rendering
/**************************************************************************/

void GLRnrDriver::BeginRender()
{
  if (mQuadricStd == 0)
  {
    mQuadricStd = gluNewQuadric();
    gluQuadricDrawStyle(mQuadricStd, GLU_FILL);
    gluQuadricNormals(mQuadricStd, GLU_SMOOTH);
    
    mQuadricStdNoNormals = gluNewQuadric();
    gluQuadricDrawStyle(mQuadricStdNoNormals, GLU_FILL);
    gluQuadricNormals(mQuadricStdNoNormals, GLU_NONE);
  }
  
  RnrDriver::BeginRender();

  if (mStatsEvery > 0) mStatsStart.SetNow();

  {
    ZTrans proj;
    glGetDoublev(GL_PROJECTION_MATRIX, proj.Array());
    glGetDoublev(GL_MODELVIEW_MATRIX,  mViewAtBegin.Array());
    SetFrustum(proj * mViewAtBegin);

    // The eye at the origin of eye coordinates, or along +z for orthographic.
    ZTrans inv(mViewAtBegin);
    inv.Invert();
    bool   ortho = proj.CM(3,2) == 0;
    Double_t e[4];
    for (int i = 0; i < 3; ++i)
      e[i] = ortho ? inv.CM(i,2) : inv.CM(i,3);
    e[3] = ortho ? 0 : 1;
    SetEyePos(e);
  }

  if(mFaderStack == 0) {
    // Make default rnrmod.
    mFaderStack = &mRMStacks.insert(std::make_pair(ZGlColorFader::FID(), RMStack())).first->second;
    ZGlass* lens = GledNS::ConstructLens(ZGlColorFader::FID());
    A_Rnr*  rnr  = lens->VGlassInfo()->SpawnRnr(mRnrName, lens);
    mFaderStack->m_def_autogen = new RnrMod(lens, rnr);
  }

  bRedraw = false;

  for(int i=0; i<mMaxLamps; ++i) {
    mLamps[i] = 0;
  }
  for(int i=0; i<mMaxClipPlanes; ++i) {
    mClipPlanes[i] = 0;
  }
  bInDLRebuild = false;
}


void GLRnrDriver::EndRender()
{
  for(int l=0; l<mMaxLamps; ++l) {
    if(mLamps[l] != 0) {
      // std::cout <<"RnrDriver cleaning-up a dirty lamp ...\n";
      mLamps[l]->CleanUp(this);
    }
  }
  for(int l=0; l<mMaxClipPlanes; ++l) {
    if(mClipPlanes[l] != 0) {
      std::cout <<"RnrDriver cleaning-up a dirty clip-plane ...\n";
      mClipPlanes[l]->CleanUp(this);
    }
  }

  RnrDriver::EndRender();

  if (bCheckPrograms)
  {
    for (GLenum e = glGetError(); e != GL_NO_ERROR; e = glGetError())
      printf("GLRnrDriver::EndRender GL error 0x%x.\n", e);
  }

  if (mStatsEvery > 0)
  {
    glFinish();
    Double_t ms = 1000 * mStatsStart.TimeUntilNow().ToDouble();
    mStatsSum += ms;
    mStatsMax  = TMath::Max(mStatsMax, ms);
    mStatsCulled      += mCullCount;
    mStatsCulledParts += mCullPartCount;
    mStatsBlended     += mBlendedCount;
    mStatsReplays     += mReplayCount;
    if (++mStatsN == mStatsEvery)
    {
      printf("GLRnrDriver stats: %d renders, mean %.2f ms, max %.2f ms, mean culled %.1f lenses, %.1f parts, mean blended %.1f in %.1f replays\n",
             mStatsN, mStatsSum / mStatsN, mStatsMax, mStatsCulled / mStatsN, mStatsCulledParts / mStatsN,
             mStatsBlended / mStatsN, mStatsReplays / mStatsN);
      mStatsN = 0;
      mStatsSum = mStatsMax = mStatsCulled = mStatsCulledParts = mStatsBlended = mStatsReplays = 0;
    }
  }
}

void GLRnrDriver::FlushBlended()
{
  // In-order draws first, in tree order; then the rest back to front by eye-space depth.
  struct ClearGuard
  {
    GLRnrDriver *fRD;
    ~ClearGuard() { fRD->clear_blended(); }
  } _clear { this };

  bDeferBlended = false;
  const UInt_t n = mBlended.size();
  if (n == 0) return;

  mBlendedOrder.clear();
  mBlendedKeys.clear();
  mBlendedIdx.clear();
  for (UInt_t i = 0; i < n; ++i)
    if ( ! mBlended[i].bSorted) mBlendedOrder.push_back(i);
  for (UInt_t i = 0; i < n; ++i)
  {
    const BlendedItem &it = mBlended[i];
    if ( ! it.bSorted) continue;
    const ZTrans &v = mViewAtBegin;
    const Float_t z = v.CM(2,0)*it.fPos[0] + v.CM(2,1)*it.fPos[1] + v.CM(2,2)*it.fPos[2] + v.CM(2,3);
    // Distance along the view is -z; map the float to a monotone key, inverted for descending.
    const UInt_t u   = std::bit_cast<UInt_t>(-z);
    const UInt_t key = (u & 0x80000000u) ? ~u : (u | 0x80000000u);
    mBlendedKeys.push_back(~key);
    mBlendedIdx.push_back(i);
  }
  // Both sorts are stable. The radix sort's fixed cost of its histograms made
  // std::stable_sort faster below about 256 keys (measured, g++ -O2).
  const UInt_t ns = mBlendedKeys.size();
  if (ns >= 256)
  {
    mBlendedRadix.Sort(mBlendedKeys, mBlendedRanks);
  }
  else
  {
    mBlendedRanks.resize(ns);
    for (UInt_t i = 0; i < ns; ++i) mBlendedRanks[i] = i;
    std::stable_sort(mBlendedRanks.begin(), mBlendedRanks.end(),
                     [this](UInt_t a, UInt_t b) { return mBlendedKeys[a] < mBlendedKeys[b]; });
  }
  for (UInt_t r : mBlendedRanks) mBlendedOrder.push_back(mBlendedIdx[r]);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  // Consecutive draws under the same mods share one replay.
  const UInt_t no = mBlendedOrder.size();
  for (UInt_t b = 0, e; b < no; b = e)
  {
    const Int_t beg = mBlended[mBlendedOrder[b]].fModBeg;
    for (e = b + 1; e < no && mBlended[mBlendedOrder[e]].fModBeg == beg; ++e) ;
    draw_blended(b, e);
  }
  glPopMatrix();
}

void GLRnrDriver::push_blended_frame(PMSEntry& pmse, ZNode* node, const ZTrans& gcs)
{
  pmse.fNode  = node;
  pmse.fLocal = gcs;
  pmse.bTo = pmse.bFrom = false;
  PushPM(pmse);
  pmse.fPrev = &mBotPMSE; // fLocal is already global.
  glLoadMatrixd((mViewAtBegin * gcs).Array());
}

void GLRnrDriver::draw_blended(UInt_t ob, UInt_t oe)
{
  // Mods are replayed by their own calls, as GL state is not data. Depth writes are
  // off unless a mod turns them on.
  const BlendedItem &first = mBlended[mBlendedOrder[ob]];
  const Int_t nm = first.fModEnd - first.fModBeg;
  if ((Int_t) mBlendedPMS.size() < nm + 1) mBlendedPMS.resize(nm + 1);
  ++mReplayCount;

  // The entries live in mBlendedPMS; pop them also when a renderer throws.
  struct PMGuard
  {
    GLRnrDriver *fRD;
    Int_t        fSize;
    ~PMGuard() { while (fRD->SizePM() > fSize) fRD->PopPM(); }
  } _pm_guard { this, SizePM() };

  glPushAttrib(GL_ALL_ATTRIB_BITS);
  glDepthMask(GL_FALSE);
  glEnable(GL_NORMALIZE);

  for (Int_t k = 0; k < nm; ++k)
  {
    const BlendedMod &m = mBlendedMods[first.fModBeg + k];
    push_blended_frame(mBlendedPMS[k], m.fNode, m.fGCS);
    if (mTraceFile) trace_op(m.fRnr, m.fKind == RnrOp::K_PreDraw ? "blended-pre" : "blended-mod");
    GLensReadHolder _lck(m.fRnr->fImg->fLens);
    if (m.fKind == RnrOp::K_PreDraw) m.fRnr->PreDraw(this);
    else                             m.fRnr->Draw(this);
  }
  for (UInt_t o = ob; o < oe; ++o)
  {
    const BlendedItem &it = mBlended[mBlendedOrder[o]];
    push_blended_frame(mBlendedPMS[nm], it.fNode, it.fGCS);
    if (mTraceFile) trace_op(it.fRnr, "blended-draw");
    GLensReadHolder _lck(it.fRnr->fImg->fLens);
    it.fRnr->Draw(this);
    PopPM();
  }
  for (Int_t k = nm - 1; k >= 0; --k)
  {
    const BlendedMod &m = mBlendedMods[first.fModBeg + k];
    if (m.fKind == RnrOp::K_PreDraw)
    {
      glLoadMatrixd((mViewAtBegin * m.fGCS).Array());
      if (mTraceFile) trace_op(m.fRnr, "blended-post");
      GLensReadHolder _lck(m.fRnr->fImg->fLens);
      m.fRnr->PostDraw(this);
    }
    PopPM();
  }

  glPopAttrib();
}

bool GLRnrDriver::modelview_is_pm_stack()
{
  ZTrans mv, exp = mViewAtBegin * ToGCS();
  glGetDoublev(GL_MODELVIEW_MATRIX, mv.Array());
  Double_t d = 0, s = 0;
  for (int i = 0; i < 16; ++i)
  {
    d = TMath::Max(d, TMath::Abs(mv[i] - exp[i]));
    s = TMath::Max(s, TMath::Abs(exp[i]));
  }
  return d <= 1e-4 * TMath::Max(s, 1.0);
}

void GLRnrDriver::check_entry_frame(A_Rnr* rnr)
{
  // The cull test assumes the GL modelview is the view times the PM stack.
  if ( ! modelview_is_pm_stack())
    printf("GLRnrDriver::check_entry_frame GL modelview differs from the PM stack at %s.\n",
           rnr->fImg->fLens->Identify().Data());
}

void GLRnrDriver::check_deferred_frame(A_Rnr* rnr)
{
  // The replay draws at the PM stack's frame; a PreDraw that moves the
  // modelview without pushing a PM entry would be drawn elsewhere.
  if ( ! modelview_is_pm_stack())
    printf("GLRnrDriver::check_deferred_frame GL modelview differs from the PM stack at blended %s.\n",
           rnr->fImg->fLens->Identify().Data());
}

void GLRnrDriver::CheckCulled(const TString& what, const Float_t* ce)
{
  // All box corners, in GL's clip coordinates, must be outside one plane.
  ZTrans mv, proj;
  glGetDoublev(GL_MODELVIEW_MATRIX,  mv.Array());
  glGetDoublev(GL_PROJECTION_MATRIX, proj.Array());
  ZTrans m = proj * mv;
  int outside[6] = { 0, 0, 0, 0, 0, 0 };
  for (int k = 0; k < 8; ++k)
  {
    Double_t p[3], q[4];
    for (int i = 0; i < 3; ++i)
      p[i] = ce[i] + ((k >> i) & 1 ? ce[3+i] : -ce[3+i]);
    for (int i = 0; i < 4; ++i)
      q[i] = m.CM(i,0)*p[0] + m.CM(i,1)*p[1] + m.CM(i,2)*p[2] + m.CM(i,3);
    for (int i = 0; i < 3; ++i)
    {
      if (q[i] < -q[3]) ++outside[2*i];
      if (q[i] >  q[3]) ++outside[2*i + 1];
    }
  }
  for (int i = 0; i < 6; ++i)
    if (outside[i] == 8) return;
  printf("GLRnrDriver::CheckCulled cull of %s not confirmed.\n", what.Data());
}

/**************************************************************************/
// Name stack / Picking
/**************************************************************************/

void GLRnrDriver::BeginPick()
{
  bInPicking = true;
  bDoPickOps = true;
  mPickCount = 0;
  mStackCopy.clear();
}

void GLRnrDriver::EndPick()
{
  bInPicking = false;
}

/**************************************************************************/

void GLRnrDriver::ClearNameStack()
{
  if(!bInPicking) return;
  int m = mStackCopy.size();
  while(m-- > 0)
    glPopName();
}

void GLRnrDriver::RestoreNameStack()
{
  if(!bInPicking) return;
  std::list<UInt_t>::iterator i = mStackCopy.begin();
  while(i != mStackCopy.end())
    glPushName(*(i++));
}

/**************************************************************************/

void GLRnrDriver::push_name(A_Rnr* rnr, void* ud)
{
  ++mPickCount;
  if(mPickCount >= mPickSize) {
    mPickSize *= 2;
    mPickVector.resize(mPickSize);
  }
  A_Rnr::NSE_t& nse(mPickVector[mPickCount]);
  nse.fRnr      = rnr;
  nse.fUserData = ud;
  glPushName(mPickCount);
  mStackCopy.push_back(mPickCount);
}

void GLRnrDriver::pop_name()
{
  mStackCopy.pop_back();
  glPopName();
}

/**************************************************************************/
// Mouse event handling
/**************************************************************************/

void GLRnrDriver::PreEventHandling(A_Rnr::Fl_Event& e)
{
  // Store some details into data members so that further event
  // handling cat pick them from here.

  if (e.fIsMouse)
  {
    if (e.fEvent == FL_DRAG)
    {
      mMDragDX = e.fX - mMouseX;
      mMDragDY = e.fY - mMouseY;
    }
    else
    {
      mMDragDX = 0;
      mMDragDY = 0;
    }

    mMouseX = e.fX;
    mMouseY = e.fY;

    if (e.fEvent == FL_PUSH && e.fButton < 4)
    {
      mMPushX[e.fButton] = mMouseX;
      mMPushY[e.fButton] = mMouseY;
    }

    if (bFixedMouseRay) return;

    float yext = TMath::Tan(0.5*TMath::DegToRad()*mZFov)*mNearClip;
    float xext = yext*mWidth/mHeight;
    float xcam = xext*(2.0f*mMouseX/mWidth  - 1.0f);
    float ycam = yext*(2.0f*mMouseY/mHeight - 1.0f);

    mMouseRayPos.Zero();
    mCamFixTrans->MultiplyIP(mMouseRayPos);

    mMouseRayDir.Set(mNearClip, -xcam, -ycam);
    mMouseRayDir.Normalize();
    mCamFixTrans->RotateIP(mMouseRayDir);
  }
}

/**************************************************************************/
// Lamps & Clipping planes
/**************************************************************************/

Int_t GLRnrDriver::GetLamp(A_Rnr* rnr)
{
  for(Int_t i=0; i<mMaxLamps; ++i) {
    if(mLamps[i] == 0) { mLamps[i] = rnr; return i; }
  }
  return -1;
}

void GLRnrDriver::ReturnLamp(Int_t lamp)
{
  mLamps[lamp] = 0;
}

/**************************************************************************/

Int_t GLRnrDriver::GetClipPlane(A_Rnr* rnr)
{
  for(Int_t i=0; i<mMaxClipPlanes; ++i) {
    if(mClipPlanes[i] == 0) { mClipPlanes[i] = rnr; return i; }
  }
  return -1;
}

void GLRnrDriver::ReturnClipPlane(Int_t clip)
{
  mClipPlanes[clip] = 0;
}

/**************************************************************************/
// Color, PointSize and LineWidth scaling
/**************************************************************************/

Float_t GLRnrDriver::Color(Float_t r, Float_t g, Float_t b, Float_t a)
{
  const ZColor& C = ColorFader().RefColorFac();
  Float_t alpha = a*C.a();
  glColor4f(r*C.r(), g*C.g(), b*C.b(), alpha);
  return alpha;
}

Float_t GLRnrDriver::PointSize(Float_t size)
{
  if(size == 0)
    return -1;
  Float_t s = size*ColorFader().GetPointSizeFac();
  glPointSize(s);
  return s;
}

Float_t GLRnrDriver::LineWidth(Float_t width)
{
  if(width == 0)
    return -1;
  Float_t w = width*ColorFader().GetLineWidthFac();
  glLineWidth(w);
  return w;
}

