// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef GledCore_RnrDriver_H
#define GledCore_RnrDriver_H

#include "A_Rnr.h"
#include "PMSEntry.h"
#include <Gled/GledNS.h>
#include <Stones/ZTrans.h>
#include <Stones/ZColor.h>

#include <deque>
#include <list>
#include <stack>
#include <string>
#include <unordered_set>

namespace gled {

/**************************************************************************/
// Rnr driver
/**************************************************************************/

class GLRnrDriver;
class ZGlBlending;

class RnrDriver : public OptoStructs::ImageConsumer
{
private:
  void _init();

protected:
  Eye*		mEye;		// X{g}

  TString	mRnrName;	// X{RGs}
  Bool_t	bUseOwnRnrs;	// X{gs} True for all but GL

  UInt_t	mRnrCount;	// X{g}
  Bool_t	bDryRun;	// X{gs} Don't render, create Rnrs

  Int_t		mMaxDepth;	// X{gs} Max render level

  //----------------------------------------------------------------
  // PositionMatrix stack & viewing transforms

  std::list<PMSEntry*> mPMStack;	// X{r}
  PMSEntry        mBotPMSE;

  ZTrans*         mCamFixTrans;  // X{gs} Transformation from camera to camera-base.
  ZTrans*         mCamBaseTrans; // X{gs} CameraBase transformation in abs ref-frame.
  ZTrans*         mCamAbsTrans;  // X{gs} Camera transformation in abs ref-frame.
  ZTrans*         mProjBase;     // X{gs} Projection matrix before cam applied.
  ZTrans*         mProjMatrix;   // X{gs} Projection matrix after cam.

  Int_t	          mWidth;        // X{g}
  Int_t	          mHeight;       // X{g}
  Float_t         mAspect;       // X{g}
  Float_t         mZFov;         // X{g}
  Float_t         mNearClip;     // X{gs}
  Float_t         mFarClip;      // X{gs}

  //----------------------------------------------------------------
  // RnrMod Stack

  struct RMStack
  {
    RnrMod*             m_def;
    RnrMod*             m_def_autogen;
    std::stack<RnrMod*> m_stack;
    RMStack() : m_def(0), m_def_autogen(0) {}

    RnrMod* get_active()
    {
      if (m_stack.empty())
        return m_def ? m_def : m_def_autogen;
      else
        return m_stack.top();
    }
  };

  typedef std::unordered_map<FID_t, RMStack>           hRMStack_t;
  typedef std::unordered_map<FID_t, RMStack>::iterator hRMStack_i;

  hRMStack_t  mRMStacks;
  hRMStack_i  mRMI;
  FID_t       mRMFid;
  bool       find_rnrmod(FID_t fid);

  //----------------------------------------------------------------
  // Local renderer stash

  typedef std::unordered_map<OptoStructs::ZGlassImg*, A_Rnr*>           hImg2Rnr_t;
  typedef std::unordered_map<OptoStructs::ZGlassImg*, A_Rnr*>::iterator hImg2Rnr_i;

  hImg2Rnr_t    mOwnRnrs;

  // Views dropped by programs while a ray may be delivered; deleted later.
  std::vector<OptoStructs::A_View*> mRetiredViews;
  void flush_retired_views();

  bool          bCheckPrograms;

  void assert_program(A_Rnr* rnr);
  bool subtree_clean(A_Rnr* rnr);

  //----------------------------------------------------------------
  // Frustum culling

  Bool_t        bCulling;       // X{gs}
  UInt_t        mCullCount;     // X{g} Lenses culled in the last render.
  UInt_t        mCullPartCount; // X{g} Parts of lenses culled by their renderers.
  Bool_t        bFrustumOK;
  Double_t      mFrustum[6][4]; // Planes in the global frame, normals inwards.
  Double_t      mEyePos[4];     // Global; a point, or with w = 0 the direction to the viewer.

  static UInt_t sFrameCount;

  virtual void check_entry_frame(A_Rnr* rnr) {}

  //----------------------------------------------------------------
  // Frame trace, written when GLED_RNR_TRACE gives a file prefix.

  FILE                      *mTraceFile;
  std::string                mTrace;
  std::unordered_set<size_t> mTraceHashes;
  Int_t                      mRnrDepth;

  void trace_op(A_Rnr* rnr, const char* op);

  //----------------------------------------------------------------
  // Blended pass: blended draws are deferred with their state and drawn after the rest.

public:
  // A mod to replay around a deferred draw: PreDraw and PostDraw, or Draw for an unscoped one.
  struct BlendedMod
  {
    A_Rnr   *fRnr;
    ZNode   *fNode;  // Top of the position stack at the mod's op.
    ZTrans   fGCS;
    UChar_t  fKind;
  };
  struct BlendedItem
  {
    A_Rnr   *fRnr;
    ZNode   *fNode;
    ZTrans   fGCS;
    Float_t  fCtr[3];          // Sort point in the lens frame: the cull-box centre or the origin.
    Int_t    fModBeg, fModEnd; // Range in mBlendedMods.
    Bool_t   bSorted;          // Back to front; otherwise in tree order, before the sorted ones.
  };

protected:
  // Mods active at the current op, in the order they were applied.
  struct ModEntry
  {
    A_Rnr       *fRnr;
    PMSEntry    *fPM;
    ZGlBlending *fBlend;              // Set through NoteBlending while the mod's op runs.
    UChar_t      fKind;               // K_PreDraw, or K_Draw for an unscoped mod.
    Int_t        fDepth;              // Render depth of the op.
    Int_t        fEndDepth, fEndOp;   // Unscoped: the post op that ends its synthesised scope.
  };
  struct CallFrame
  {
    const vRnrOp_t *fOps;
    Int_t           fOp;
  };

  std::vector<ModEntry>    mMods;
  std::vector<CallFrame>   mCalls;
  Int_t                    mNoteTarget;
  std::vector<BlendedItem> mBlended;
  std::vector<BlendedMod>  mBlendedMods;
  std::deque<PMSEntry>     mBlendedPMS; // Position entries for the replay; a deque does not move them.
  Bool_t                   bBlendedPass;   // X{gs} Defer blended draws; GLED_RNR_BLENDED=0 turns it off.
  Bool_t                   bDeferBlended;  // Until FlushBlended in a render.
  UInt_t                   mBlendedCount;  // X{g} Draws deferred in the last render.

  void push_unscoped(A_Rnr* rnr, Int_t depth);
  void pop_mod(A_Rnr* rnr, Int_t depth);
  void end_unscoped(Int_t depth, Int_t op);
  void end_program(Int_t depth);
  void defer_blended(A_Rnr* rnr);
  void clear_blended();

  virtual bool can_defer() { return false; }

public:
  RnrDriver(Eye* e, const TString& r);
  virtual ~RnrDriver();

  void SetWidthHeight(Int_t w, Int_t h);
  void SetZFov(Float_t zfov);

  // ImageConsumer
  virtual void ImageDeath(OptoStructs::ZGlassImg* img);

  void FillRnrScheme(RnrScheme* rs, A_Rnr* rnr,
		     const GledNS::RnrBits& bits);
  void FillRnrScheme(RnrScheme* rs, OptoStructs::lpZGlassImg_t* imgs,
		     const GledNS::RnrBits& bits);

  void RetireView(OptoStructs::A_View* v) { mRetiredViews.push_back(v); }

  virtual A_Rnr* GetLensRnr(ZGlass* lens);
  virtual A_Rnr* AssertDefRnr(OptoStructs::ZGlassImg* img);
  virtual A_Rnr* GetRnr(OptoStructs::ZGlassImg* img)
  { return img->fDefRnr ? img->fDefRnr : AssertDefRnr(img); }

  virtual void Render(A_Rnr* img);

  void SetFrustum(const ZTrans& clip_from_global);
  bool IsVisible(const Float_t* ctr_ext);

  bool CullingActive() const { return bCulling && bFrustumOK; }
  bool CheckMode()     const { return bCheckPrograms; }
  void AddCulledParts(Int_t n) { mCullPartCount += n; }
  virtual void CheckCulled(const TString& what, const Float_t* ctr_ext) {}

  void SetEyePos(const Double_t* eye) { for (int i = 0; i < 4; ++i) mEyePos[i] = eye[i]; }
  void GetLocalEyePos(Double_t* eye);

  virtual void BeginRender();
  virtual void EndRender();

  // Called by ZGlBlending renderers; attaches the blending to the mod being applied.
  void         NoteBlending(ZGlBlending* b) { if (mNoteTarget >= 0) mMods[mNoteTarget].fBlend = b; }
  ZGlBlending* ActiveBlending();
  bool         BlendingOn();
  // Draws the deferred blended draws; later draws in the render are not deferred.
  virtual void FlushBlended() { clear_blended(); }

  // Position Matrix Stack
  void PushPM(PMSEntry& pmse) { pmse.fPrev = mPMStack.back(); mPMStack.push_back(&pmse); }
  void PopPM()                { mPMStack.pop_back(); }

  PMSEntry& TopPM() 	{ return *mPMStack.back(); }
  ZNode*    TopPMNode() { return  mPMStack.back()->fNode; }

  int  SizePM()  { return mPMStack.size(); }
  void ClearPM() { mPMStack.clear(); }

  ZTrans& ToGCS()   { return mPMStack.back()->ToGCS(); }
  ZTrans& FromGCS() { return mPMStack.back()->FromGCS(); }

  //----------------------------------------------------------------

  void    SetDefRnrMod(FID_t fid, RnrMod* ud);
  RnrMod* GetDefRnrMod(FID_t fid);
  void    PushRnrMod(FID_t fid, RnrMod* ud);
  RnrMod* PopRnrMod(FID_t fid);
  RnrMod* TopRnrMod(FID_t fid);
  RnrMod* GetRnrMod(FID_t fid);

  void RemoveRnrModEntry(FID_t fid);
  void CleanUpRnrModDefaults();

  //----------------------------------------------------------------

  GLRnrDriver* GL() { return (GLRnrDriver*)this; }

#include "RnrDriver.h7"
}; // endclass RnrDriver


/**************************************************************************/
// Inlines
/**************************************************************************/

inline bool RnrDriver::find_rnrmod(FID_t fid)
{
  if (fid == mRMFid) return true;
  mRMI = mRMStacks.find(fid);
  if (mRMI == mRMStacks.end())
  {
    mRMFid.clear();
    return false;
  }
  else
  {
    mRMFid = fid;
    return true;
  }
}

/**************************************************************************/
// Preprocessor shortcuts for render-mod access.
/**************************************************************************/

#define RNRDRIVER_GET_RNRMOD_BOTH(_var_, _rd_, _typ_) \
  RnrMod* _var_ = _rd_->GetRnrMod(_typ_::FID()); \
  _typ_* _var_ ## _lens = (_typ_*) _var_->fLens; \
  _typ_ ## _GL_Rnr* _var_ ## _rnr = (_typ_ ## _GL_Rnr*) _var_->fRnr

#define RNRDRIVER_GET_RNRMOD_LENS(_var_, _rd_, _typ_) \
  RnrMod* _var_ = _rd_->GetRnrMod(_typ_::FID()); \
  _typ_* _var_ ## _lens = (_typ_*) _var_->fLens

#define RNRDRIVER_GET_RNRMOD_RNR(_var_, _rd_, _typ_) \
  RnrMod* _var_ = _rd_->GetRnrMod(_typ_::FID()); \
  _typ_ ## _GL_Rnr* _var_ ## _rnr = (_typ_ ## _GL_Rnr*) _var_->fRnr

#define RNRDRIVER_GET_RNRMOD(_var_, _rd_, _typ_) \
  RnrMod* _var_ = _rd_->GetRnrMod(_typ_::FID())


#define RNRDRIVER_CAST_RNRMOD_BOTH(_var_, _rnrmod_, _typ_) \
  _typ_* _var_ ## _lens = (_typ_*) _rnrmod_->fLens; \
  _typ_ ## _GL_Rnr* _var_ ## _rnr = (_typ_ ## _GL_Rnr*) _rnrmod_->fRnr

#define RNRDRIVER_CAST_RNRMOD_LENS(_var_, _rnrmod_, _typ_) \
  _typ_* _var_ ## _lens = (_typ_*) _rnrmod_->fLens

#define RNRDRIVER_CAST_RNRMOD_RNR(_var_, _rnrmod_, _typ_) \
  _typ_ ## _GL_Rnr* _var_ ## _rnr = (_typ_ ## _GL_Rnr*) _rnrmod_->fRnr


} // endnamespace gled

#endif
