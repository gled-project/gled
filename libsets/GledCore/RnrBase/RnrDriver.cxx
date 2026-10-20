// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

//__________________________________________________________________________
// RnrDriver
//
// Generic RnrDriver implementation.
// Currently still GL-only, as infrastructure for POV-Ray rendering is not
// yet prepared.

#include "RnrDriver.h"
#include <Eye/Eye.h>
#include <Glasses/ZGlass.h>
#include <Glasses/ZNode.h>
#include <Glasses/ZGlBlending.h>

#include <algorithm>
#include <cmath>
#include <cstdlib>

using namespace gled;


namespace OS   = OptoStructs;
namespace GVNS = GledViewNS;

/**************************************************************************/

RnrDriver::RnrDriver(Eye* e, const TString& r) : mEye(e), mRnrName(r)
{
  bUseOwnRnrs = false;

  mRnrCount = 0;
  bDryRun   = false;

  mMaxDepth = 100;

  mBotPMSE.bTo = true; mBotPMSE.bFrom = true; // Bottom PMSE holds the identity.

  bCheckPrograms = getenv("GLED_RNR_CHECK") != 0;

  const char* cull = getenv("GLED_RNR_CULL");
  bCulling   = ! (cull && cull[0] == '0');
  mCullCount = mCullPartCount = 0;
  bFrustumOK = false;
  mEyePos[0] = mEyePos[1] = mEyePos[3] = 0; mEyePos[2] = 1;

  const char* blended = getenv("GLED_RNR_BLENDED");
  bBlendedPass  = ! (blended && blended[0] == '0');
  bDeferBlended = false;
  mBlendedCount = mReplayCount = 0;
  mNoteTarget   = -1;
  mModGen = mDeferGen = 0;

  mTraceFile = 0;
  mRnrDepth  = 0;
  if (const char* prefix = getenv("GLED_RNR_TRACE"))
  {
    static int s_count = 0;
    mTraceFile = fopen(GForm("%s.%d.trace", prefix, s_count++), "w");
  }

  mEye->RegisterImageConsumer(this);
}

RnrDriver::~RnrDriver()
{
  if (mTraceFile) fclose(mTraceFile);
  flush_retired_views();
  mEye->UnregisterImageConsumer(this);
}

void RnrDriver::flush_retired_views()
{
  for (auto v : mRetiredViews) delete v;
  mRetiredViews.clear();
}

/**************************************************************************/

void RnrDriver::SetWidthHeight(Int_t w, Int_t h)
{
  mWidth  = w;
  mHeight = h;
  mAspect = (Float_t) w / h;
}

void RnrDriver::SetZFov(Float_t zfov)
{
  mZFov = zfov;
}

/**************************************************************************/

void RnrDriver::ImageDeath(OS::ZGlassImg* img)
{
  // The image deletes its views, retired ones included.
  mRetiredViews.erase(std::remove_if(mRetiredViews.begin(), mRetiredViews.end(),
                                     [img](OS::A_View* v) { return v->fImg == img; }),
                      mRetiredViews.end());

  hImg2Rnr_i oi = mOwnRnrs.find(img);
  if (oi != mOwnRnrs.end()) {
    delete oi->second;
    mOwnRnrs.erase(oi);
  }
}

/**************************************************************************/

void RnrDriver::FillRnrScheme(RnrScheme* rs, A_Rnr* rnr,
		   const GledNS::RnrBits& bits)
{
  rs->Add(bits.fSelf[0], rnr, RnrOp::K_PreDraw);
  rs->Add(bits.fSelf[1], rnr, RnrOp::K_Draw);
  rs->Add(bits.fSelf[2], rnr, RnrOp::K_PostDraw);
  rs->Add(bits.fSelf[3], rnr, RnrOp::K_Call);
}

void RnrDriver::FillRnrScheme(RnrScheme* rs, OS::lpZGlassImg_t* imgs,
		   const GledNS::RnrBits& bits)
{
  static const UChar_t kinds[4] = { RnrOp::K_PreDraw, RnrOp::K_Draw, RnrOp::K_PostDraw, RnrOp::K_Call };
  for (int k = 0; k < 4; ++k)
  {
    if (bits.fList[k])
      for (OS::lpZGlassImg_i img=imgs->begin(); img!=imgs->end(); ++img)
        rs->Add(bits.fList[k], GetRnr(*img), kinds[k]);
  }
}

/**************************************************************************/
/**************************************************************************/

A_Rnr* RnrDriver::GetLensRnr(ZGlass* lens)
{
  // This one could be extended to use internal/stack-dependant
  // resolution and thus support multiple renderers.

  OS::ZGlassImg* img = mEye->DemanglePtr(lens);
  if (img) return AssertDefRnr(img);
  return 0;
}

A_Rnr* RnrDriver::AssertDefRnr(OS::ZGlassImg* img)
{
  // This one is somewhat tricky: A_Rnr is A_View, but constructed
  // with 0 image. Thus we set it after.

  if (img->fDefRnr != 0) return img->fDefRnr;

  hImg2Rnr_i oi = mOwnRnrs.find(img);
  if (oi != mOwnRnrs.end()) return oi->second;

  A_Rnr* rnr = img->GetCI()->SpawnRnr(mRnrName, img->fLens);
  assert(rnr != 0);
  rnr->SetImg(img);

  if (rnr->bOnePerRnrDriver)
    mOwnRnrs[img] = rnr;
  else
    img->fDefRnr = rnr;

  return rnr;
}

UInt_t RnrDriver::sFrameCount = 0;

void RnrDriver::assert_program(A_Rnr* rnr)
{
  static const Exc_t _eh("RnrDriver::assert_program ");

  if (rnr->mProgram == 0)
    rnr->mProgram = new RnrProgram;
  RnrProgram &prog = *rnr->mProgram;

  if (prog.bDirty || bCheckPrograms)
  {
    RnrScheme sch;
    {
      GLensReadHolder _lck(rnr->fImg->fLens);
      rnr->CreateRnrScheme(this, &sch);
    }
    if (prog.bDirty)
    {
      prog.Set(this, sch);
    }
    else if ( ! prog.Matches(sch))
    {
      printf("%sstale program of %s.\n", _eh.Data(), rnr->fImg->fLens->Identify().Data());
      prog.Set(this, sch);
    }
  }
}

bool RnrDriver::subtree_clean(A_Rnr* rnr)
{
  if (rnr->mProgram == 0 || rnr->mProgram->bDirty)
    assert_program(rnr);
  RnrProgram &prog = *rnr->mProgram;

  if (prog.fCleanFrame == sFrameCount)
    return prog.bClean;
  prog.fCleanFrame = sFrameCount;
  prog.bClean      = false; // A cycle counts as not clean.

  if (prog.bUnpaired)
    return false;
  for (const RnrOp &op : prog.fOps)
  {
    if (op.fKind == RnrOp::K_Call ? ! subtree_clean(op.fRnr) :
        op.fKind == RnrOp::K_Draw && op.fRnr->HasOutsideEffects())
      return false;
  }
  return prog.bClean = true;
}

namespace
{
  // Cleared also when a renderer throws; Pupil::Render catches.
  struct InRenderGuard
  {
    bool &fFlag;
    InRenderGuard(bool& f) : fFlag(f) { fFlag = true; }
    ~InRenderGuard() { fFlag = false; }
  };

  template<class T>
  struct PopBackGuard
  {
    std::vector<T> &fVec;
    PopBackGuard(std::vector<T>& v) : fVec(v) {}
    ~PopBackGuard() { if ( ! fVec.empty()) fVec.pop_back(); }
  };
}

void RnrDriver::Render(A_Rnr* rnr)
{
  if (mMaxDepth <= 0) { if (mTraceFile) trace_op(rnr, "cut"); return; }

  // Reached again inside its own subtree: its position entry would be pushed twice.
  if (rnr->bInRender)
  {
    if ( ! rnr->bCycleReported)
    {
      printf("RnrDriver::Render cycle at %s, not entered again.\n", rnr->fImg->fLens->Identify().Data());
      rnr->bCycleReported = true;
    }
    if (mTraceFile) trace_op(rnr, "cycle");
    return;
  }

  assert_program(rnr);

  if (bCulling && bFrustumOK)
  {
    Float_t box[6];
    bool    has_box;
    {
      GLensReadHolder _lck(rnr->fImg->fLens);
      has_box = rnr->GetCullBox(this, box);
    }
    if (has_box)
    {
      if (bCheckPrograms) check_entry_frame(rnr);
      if ( ! IsVisible(box) && subtree_clean(rnr))
      {
        if (bCheckPrograms) CheckCulled(rnr->fImg->fLens->Identify(), box);
        if (mTraceFile) trace_op(rnr, "culled");
        ++mCullCount;
        return;
      }
    }
  }

  InRenderGuard _guard(rnr->bInRender);
  --mMaxDepth;
  if (mTraceFile) { trace_op(rnr, "enter"); ++mRnrDepth; }

  const vRnrOp_t &ops   = rnr->mProgram->fOps;
  const Int_t     n     = ops.size();
  const Int_t     depth = mCalls.size();
  mCalls.push_back({ &ops, 0, mPMStack.back() });
  PopBackGuard<CallFrame> _call_guard(mCalls);

  for (Int_t i = 0; i < n; ++i)
  {
    const RnrOp &op = ops[i];
    mCalls[depth].fOp = i;
    if ( ! mMods.empty()) end_unscoped(depth, i);

    if (op.fKind == RnrOp::K_Call)
    {
      Render(op.fRnr);
      continue;
    }

    GLensReadHolder _lck(op.fRnr->fImg->fLens);
    switch (op.fKind)
    {
      case RnrOp::K_PreDraw:
      {
        if (mTraceFile) trace_op(op.fRnr, "pre");
        // Pre ops of other renderers are mods (links, rnr-mods); the lens's own pre is its frame.
        if (op.fRnr != rnr)
        {
          push_mod(op.fRnr, RnrOp::K_PreDraw, depth);
          mNoteTarget = mMods.size() - 1;
        }
        op.fRnr->PreDraw(this);
        mNoteTarget = -1;
        break;
      }
      case RnrOp::K_Draw:
      {
        if (op.fRnr->HasOutsideEffects())
        {
          if (mTraceFile) trace_op(op.fRnr, "draw");
          push_unscoped(op.fRnr, depth);
          mNoteTarget = mMods.size() - 1;
          op.fRnr->Draw(this);
          mNoteTarget = -1;
        }
        else if (can_defer() && op.fRnr->IsBlended(this) && may_defer())
        {
          if (mTraceFile) trace_op(op.fRnr, "defer");
          defer_blended(op.fRnr, rnr, depth);
        }
        else
        {
          if (mTraceFile) trace_op(op.fRnr, "draw");
          op.fRnr->Draw(this);
        }
        break;
      }
      case RnrOp::K_PostDraw:
      {
        if (mTraceFile) trace_op(op.fRnr, "post");
        op.fRnr->PostDraw(this);
        if (op.fRnr != rnr && ! mMods.empty()) pop_mod(op.fRnr, depth);
        break;
      }
    }
  }
  if ( ! mMods.empty()) end_program(depth);

  if (mTraceFile) --mRnrDepth;
  ++mMaxDepth;
}

/**************************************************************************/
// Blended pass
/**************************************************************************/

void RnrDriver::push_mod(A_Rnr* rnr, UChar_t kind, Int_t depth)
{
  mMods.push_back({ rnr, mPMStack.back(), 0, kind, ! rnr->IsReplayable(this), depth, -1, -1 });
  ++mModGen;
}

void RnrDriver::push_unscoped(A_Rnr* rnr, Int_t depth)
{
  // An unscoped mod lasts until the post op of the innermost enclosing pair,
  // here or in a caller; with none, until the end of the render.
  push_mod(rnr, RnrOp::K_Draw, depth);
  ModEntry &e = mMods.back();
  for (Int_t d = depth; d >= 0 && e.fEndDepth < 0; --d)
  {
    const vRnrOp_t &o = *mCalls[d].fOps;
    const Int_t     i = mCalls[d].fOp;
    for (Int_t p = i - 1; p >= 0; --p)
    {
      if (o[p].fKind == RnrOp::K_PreDraw && o[p].fPair > i)
      {
        e.fEndDepth = d;
        e.fEndOp    = o[p].fPair;
        break;
      }
    }
  }
}

void RnrDriver::pop_mod(A_Rnr* rnr, Int_t depth)
{
  // Pairs can cross (pre A, pre B, post A, post B), so the entry need not be on top.
  for (Int_t k = mMods.size() - 1; k >= 0; --k)
  {
    const ModEntry &e = mMods[k];
    if (e.fRnr == rnr && e.fKind == RnrOp::K_PreDraw && e.fDepth == depth)
    {
      mMods.erase(mMods.begin() + k);
      ++mModGen;
      return;
    }
  }
}

void RnrDriver::end_unscoped(Int_t depth, Int_t op)
{
  auto i = std::remove_if(mMods.begin(), mMods.end(), [=](const ModEntry& e)
                          { return e.fKind == RnrOp::K_Draw && e.fEndDepth == depth && e.fEndOp == op; });
  if (i != mMods.end()) { mMods.erase(i, mMods.end()); ++mModGen; }
}

void RnrDriver::end_program(Int_t depth)
{
  // Unpaired pre ops end with their program.
  auto i = std::remove_if(mMods.begin(), mMods.end(), [=](const ModEntry& e)
                          { return e.fKind == RnrOp::K_PreDraw ? e.fDepth >= depth : e.fEndDepth >= depth; });
  if (i != mMods.end()) { mMods.erase(i, mMods.end()); ++mModGen; }
}

ZGlBlending* RnrDriver::ActiveBlending()
{
  for (auto i = mMods.rbegin(); i != mMods.rend(); ++i)
    if (i->fBlend) return i->fBlend;
  return 0;
}

bool RnrDriver::BlendingOn()
{
  ZGlBlending *b = ActiveBlending();
  return b && b->GetBlendOp() == ZRnrModBase::O_On;
}

bool RnrDriver::may_defer()
{
  // Not under a mod that cannot be replayed, nor under a blending drawn in place.
  ZGlBlending *b = 0;
  for (auto i = mMods.rbegin(); i != mMods.rend(); ++i)
  {
    if (i->bInPlace) return false;
    if ( ! b) b = i->fBlend;
  }
  return ! b || b->GetRnrAlgo() != ZGlBlending::RA_InPlace;
}

void RnrDriver::defer_blended(A_Rnr* rnr, A_Rnr* owner, Int_t depth)
{
  BlendedItem it;
  it.fRnr  = rnr;
  it.fNode = TopPMNode();
  it.fGCS  = ToGCS();
  // The cull box is in the frame at the lens's entry; a lens drawn through a link has none.
  Float_t box[6];
  if (rnr == owner && rnr->GetCullBox(this, box))
  {
    const ZTrans &t = mCalls[depth].fEntryPM->ToGCS();
    for (int i = 0; i < 3; ++i)
      it.fPos[i] = t.CM(i,0)*box[0] + t.CM(i,1)*box[1] + t.CM(i,2)*box[2] + t.CM(i,3);
  }
  else
  {
    for (int i = 0; i < 3; ++i) it.fPos[i] = it.fGCS.CM(i,3);
  }
  ZGlBlending *b = ActiveBlending();
  it.bSorted = ! b || b->GetRnrAlgo() == ZGlBlending::RA_Sorted;
  if ( ! mBlended.empty() && mDeferGen == mModGen)
  {
    it.fModBeg = mBlended.back().fModBeg;
    it.fModEnd = mBlended.back().fModEnd;
  }
  else
  {
    it.fModBeg = mBlendedMods.size();
    for (const ModEntry &e : mMods)
      mBlendedMods.push_back({ e.fRnr, e.fPM->fNode, e.fPM->ToGCS(), e.fKind });
    it.fModEnd = mBlendedMods.size();
    mDeferGen = mModGen;
  }
  mBlended.push_back(it);
  ++mBlendedCount;
}

void RnrDriver::clear_blended()
{
  bDeferBlended = false;
  mBlended.clear();
  mBlendedMods.clear();
  mMods.clear();
  mCalls.clear();
  ++mModGen;
}

void RnrDriver::SetFrustum(const ZTrans& m)
{
  // Gribb and Hartmann: plane = row 3 +- row i of the clip matrix.
  for (int i = 0; i < 3; ++i)
  {
    for (int j = 0; j < 4; ++j)
    {
      mFrustum[2*i    ][j] = m.CM(3, j) + m.CM(i, j);
      mFrustum[2*i + 1][j] = m.CM(3, j) - m.CM(i, j);
    }
  }
  bFrustumOK = true;
}

bool RnrDriver::IsVisible(const Float_t* ce)
{
  // Global axis-aligned box enclosing the local one.
  const ZTrans &t = ToGCS();
  Double_t c[3], e[3];
  for (int i = 0; i < 3; ++i)
  {
    c[i] = t.CM(i, 3);
    e[i] = 0;
    for (int j = 0; j < 3; ++j)
    {
      c[i] += t.CM(i, j) * ce[j];
      e[i] += std::abs(t.CM(i, j)) * ce[3 + j];
    }
  }
  for (int p = 0; p < 6; ++p)
  {
    const Double_t *n = mFrustum[p];
    if (n[0]*c[0] + n[1]*c[1] + n[2]*c[2] + n[3] +
        std::abs(n[0])*e[0] + std::abs(n[1])*e[1] + std::abs(n[2])*e[2] < 0)
      return false;
  }
  return true;
}

void RnrDriver::GetLocalEyePos(Double_t* e)
{
  // In the frame on top of the position stack.
  const ZTrans &t = FromGCS();
  for (int i = 0; i < 3; ++i)
    e[i] = t.CM(i,0)*mEyePos[0] + t.CM(i,1)*mEyePos[1] + t.CM(i,2)*mEyePos[2] + t.CM(i,3)*mEyePos[3];
  e[3] = mEyePos[3];
}

void RnrDriver::trace_op(A_Rnr* rnr, const char* op)
{
  ZGlass *lens = rnr->fImg->fLens;
  mTrace += GForm("%*s%s %d %s '%s'\n", 2*mRnrDepth, "", op, lens->GetSaturnID(),
                  lens->VGlassInfo()->fName.Data(), lens->GetName());
}

/**************************************************************************/
/**************************************************************************/

void RnrDriver::BeginRender()
{
  ++mRnrCount;
  ++sFrameCount;
  mCullCount = mCullPartCount = 0;
  bFrustumOK = false;
  mPMStack.push_back(&mBotPMSE);
  if ( ! mRetiredViews.empty() && mEye->GetCurrentRay() == 0)
    flush_retired_views();
  if (mTraceFile) { mTrace.clear(); mRnrDepth = 0; }
  clear_blended();
  bDeferBlended = bBlendedPass;
  mBlendedCount = mReplayCount = 0;
  mNoteTarget   = -1;
}


void RnrDriver::EndRender()
{
  mPMStack.pop_back();
  if (mTraceFile && mTraceHashes.insert(std::hash<std::string>()(mTrace)).second)
  {
    fprintf(mTraceFile, "=== frame\n%s", mTrace.c_str());
    fflush(mTraceFile);
  }
}

/**************************************************************************/
// RnrMods interface
/**************************************************************************/

void RnrDriver::SetDefRnrMod(FID_t fid, RnrMod* ud)
{
  if (find_rnrmod(fid)) {
    mRMI->second.m_def = ud;
  } else {
    mRMStacks[fid].m_def = ud;
  }
}

RnrMod* RnrDriver::GetDefRnrMod(FID_t fid)
{
  if (find_rnrmod(fid)) {
    return mRMI->second.m_def;
  } else {
    return 0;
  }
}

void RnrDriver::PushRnrMod(FID_t fid, RnrMod* ud)
{
  if (find_rnrmod(fid)) {
    mRMI->second.m_stack.push(ud);
  } else {
    mRMStacks[fid].m_stack.push(ud);
  }
}

RnrMod* RnrDriver::PopRnrMod(FID_t fid)
{
  if (find_rnrmod(fid) && ! mRMI->second.m_stack.empty()) {
    RnrMod* r = mRMI->second.m_stack.top();
    mRMI->second.m_stack.pop();
    return r;
  } else {
    return 0;
  }
}

RnrMod* RnrDriver::TopRnrMod(FID_t fid)
{
  if (find_rnrmod(fid) && ! mRMI->second.m_stack.empty()) {
    return mRMI->second.m_stack.top();
  } else {
    return 0;
  }
}

RnrMod* RnrDriver::GetRnrMod(FID_t fid)
{
  if (find_rnrmod(fid) == false) {
    mRMI = mRMStacks.insert(std::make_pair(fid, RMStack())).first;
    mRMFid = fid;
  }

  if (mRMI->second.m_stack.empty()) {
    if (mRMI->second.m_def) return mRMI->second.m_def;
    if (mRMI->second.m_def_autogen == 0) {
      ZGlass* lens = GledNS::ConstructLens(fid);
      A_Rnr*  rnr  = lens->VGlassInfo()->SpawnRnr(mRnrName, lens);
      mRMI->second.m_def_autogen = new RnrMod(lens, rnr);
    }
    return mRMI->second.m_def_autogen;
  } else {
    return mRMI->second.m_stack.top();
  }
}

/**************************************************************************/

void RnrDriver::RemoveRnrModEntry(FID_t fid)
{
  if (find_rnrmod(fid)) {
    if (mRMI->second.m_def_autogen != 0) {
      RnrMod* rm = mRMI->second.m_def_autogen;
      delete rm->fRnr;
      delete rm->fLens;
      delete rm;
    }
    mRMStacks.erase(mRMI); mRMFid.clear();
  }
}

void RnrDriver::CleanUpRnrModDefaults()
{
  static const Exc_t _eh("RnrDriver::CleanUpRnrModDefaults ");

  for (hRMStack_i i=mRMStacks.begin(); i!=mRMStacks.end(); ++i) {
    i->second.m_def = 0;
    int cnt = 0;
    while ( ! i->second.m_stack.empty()) {
      ++cnt;
      i->second.m_stack.pop();
    }
    if (cnt)
      printf("%sstack for '%s' not empty (%d entries remained).",
	     _eh.Data(), GledNS::FindClassInfo(i->first)->fName.Data(), cnt);
  }
}
