// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef GledCore_GLRnrDriver_H
#define GledCore_GLRnrDriver_H

#include <RnrBase/RnrDriver.h>
#include <Gled/GTime.h>
#include <Stones/RadixSort.h>

#include <GL/glew.h>

namespace gled {

class ZGlColorFader;

class GLRnrDriver : public RnrDriver
{
protected:
  Bool_t                    bInPicking;  // X{G}
  Bool_t                    bDoPickOps;  // X{gs}

  UInt_t                    mPickCount;  // X{g}
  UInt_t                    mPickSize;
  A_Rnr::vNSE_t             mPickVector; // X{r}
  std::list<UInt_t>              mStackCopy;

  void push_name(A_Rnr* rnr, void* ud);
  void pop_name();

  A_Rnr*                    mBelowMouse; // X{gs}
  A_Rnr*                    mPushed;     // X{gs}
  A_Rnr*                    mFocus;      // X{gs}

  Int_t                     mMouseX;     // X{g}
  Int_t                     mMouseY;     // X{g}
  Int_t                     mMPushX[4];
  Int_t                     mMPushY[4];
  Int_t                     mMDragDX;    // X{g}
  Int_t                     mMDragDY;    // X{g}

  ZPoint                    mMouseRayPos; // X{R}
  ZPoint                    mMouseRayDir; // X{R}
  bool                      bFixedMouseRay; // Set by a headset pupil, not from the mouse.

  //--------------------------------

  Int_t		mMaxLamps;	 // X{g}
  A_Rnr**	mLamps;

  Int_t		mMaxClipPlanes;  // X{g}
  A_Rnr**	mClipPlanes;

  Bool_t        bInDLRebuild;    // X{gs}
  Bool_t        bMarkupNodes;	 // X{gs}

  Bool_t        bRedraw;         // X{gs}

  RMStack*      mFaderStack;

  //--------------------------------

  GLUquadricObj *mQuadricStd;          // X{g}
  GLUquadricObj *mQuadricStdNoNormals; // X{g}

  //--------------------------------

  RadixSort<UInt_t, UInt_t> mBlendedRadix;
  std::vector<UInt_t>       mBlendedKeys, mBlendedRanks, mBlendedIdx, mBlendedOrder;

  // Selection has no blending or depth order, so blended draws are picked in place.
  virtual bool can_defer() { return bDeferBlended && ! bInPicking && ! bInDLRebuild; }

  void push_blended_frame(PMSEntry& pmse, ZNode* node, const ZTrans& gcs);
  void draw_blended(UInt_t ob, UInt_t oe);

public:
  GLRnrDriver(Eye* e, const TString& r);
  virtual ~GLRnrDriver();

  virtual void BeginRender();
  virtual void EndRender();
  virtual void FlushBlended();

  // A ray in the camera-base frame that events use in place of the mouse's.
  void SetFixedMouseRay(const ZPoint& pos, const ZPoint& dir)
  { mMouseRayPos = pos; mMouseRayDir = dir; bFixedMouseRay = true; }
  void ReleaseFixedMouseRay() { bFixedMouseRay = false; }

  ZTrans        mViewAtBegin;

  // Render times, printed every mStatsEvery renders when GLED_RNR_STATS gives it.
  Int_t         mStatsEvery, mStatsN;
  Double_t      mStatsSum, mStatsMax, mStatsCulled, mStatsCulledParts, mStatsBlended, mStatsReplays;
  GTime         mStatsStart;
  virtual void  check_entry_frame(A_Rnr* rnr);
  virtual void  CheckCulled(const TString& what, const Float_t* ctr_ext);

  // NameStack
  virtual void BeginPick();
  virtual void EndPick();

  A_Rnr::NSE_t& NameStack(UInt_t i)
  { if(i>mPickCount) i=0; return mPickVector[i]; }

  Bool_t PickingP()                     { return bInPicking && bDoPickOps; }
  void PushName(A_Rnr* rnr, void* ud=0) { if (PickingP()) push_name(rnr, ud); }
  void PopName()                        { if (PickingP()) pop_name(); }

  void ClearNameStack();
  void RestoreNameStack();

  // Event handling
  void  PreEventHandling(A_Rnr::Fl_Event& e);

  Int_t GetMPushX(Int_t b) const { return mMPushX[b]; }
  Int_t GetMPushY(Int_t b) const { return mMPushY[b]; }

  // Lamps
  A_Rnr** GetLamps() { return mLamps; }
  Int_t   GetLamp(A_Rnr* rnr);
  void    ReturnLamp(Int_t lamp);

  // Clipping planes
  A_Rnr** GetClipPlanes() { return mClipPlanes; }
  Int_t   GetClipPlane(A_Rnr* rnr);
  void    ReturnClipPlane(Int_t lamp);

  // Color, PointSize and LineWidth scaling
  Float_t Color(Float_t r, Float_t g, Float_t b, Float_t a=1);
  Float_t Color(const ZColor& c) { return Color(c.r(), c.g(), c.b(), c.a()); }
  Float_t PointSize(Float_t size);
  Float_t LineWidth(Float_t width);

  ZGlColorFader& ColorFader()
  { return (ZGlColorFader&) *mFaderStack->get_active()->fLens; }

#include "GLRnrDriver.h7"
}; // endclass GLRnrDriver

} // endnamespace gled

#endif
