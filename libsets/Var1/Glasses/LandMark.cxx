// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "LandMark.h"

#include "Tringula.h"
#include "ParaSurf.h"

using namespace gled;

#include "LandMark.c7"

// LandMark

//______________________________________________________________________________
//
//

//==============================================================================

void LandMark::_init()
{
  mF = mG = mH = mHRel = 0;
  mHeightMode = HM_Absolute;
  mPhi = 0;
  mSx = mSy = 1;
  mColor.rgba(1, 0, 0.3, 0.5);
  mLineHeight    = 0.1;
  mNLineSegments = 64;
  bDetailedLine  = true;
  bRnrIdealLine  = true;

  bUseDispList = true;
}

LandMark::LandMark(const Text_t* n, const Text_t* t) :
  Extendio(n, t)
{
  _init();
}

LandMark::~LandMark()
{}

//==============================================================================

void LandMark::retrans(ParaSurf* ps)
{
  ps->regularize_fg(&mF);
  ps->fgh2trans(&mF, mTrans);
  mTrans.RotateLF(1, 2, TMath::DegToRad()*mPhi);
}

//==============================================================================

void LandMark::SetF(Float_t f)
{
  ParaSurf* ps = mTringula->GetParaSurf();

  if (f < ps->GetMinF()) f = ps->GetMinF();
  if (f > ps->GetMaxF()) f = ps->GetMaxF();
  mF = f;

  retrans(ps);
  StampReqTring(FID()); 
}

void LandMark::SetG(Float_t g)
{
  ParaSurf* ps = mTringula->GetParaSurf();

  if (g < ps->GetMinG()) g = ps->GetMinG();
  if (g > ps->GetMaxG()) g = ps->GetMaxG();
  mG = g;

  retrans(ps);
  StampReqTring(FID()); 
}

void LandMark::SetH(Float_t h)
{
  ParaSurf* ps = mTringula->GetParaSurf();

  mH = h;

  retrans(ps);
  StampReqTring(FID()); 
}

void LandMark::SetHRel(Float_t hr)
{
  ParaSurf* ps = mTringula->GetParaSurf();

  mHRel = hr;

  retrans(ps);
  StampReqTring(FID()); 
}

void LandMark::SetFGH(Float_t f, Float_t g, Float_t h)
{
  ParaSurf* ps = mTringula->GetParaSurf();

  if (f < ps->GetMinF()) f = ps->GetMinF();
  if (f > ps->GetMaxF()) f = ps->GetMaxF();
  mF = f;

  if (g < ps->GetMinG()) g = ps->GetMinG();
  if (g > ps->GetMaxG()) g = ps->GetMaxG();
  mG = g;

  mH = h;

  retrans(ps);
  StampReqTring(FID()); 
}

void LandMark::SetPhi(Float_t p)
{
  ParaSurf* ps = mTringula->GetParaSurf();

  if (p < 0) p = 360 + p;
  if (p > 360) p = p - TMath::Floor(p / 360)*360;
  mPhi = p;

  retrans(ps);
  StampReqTring(FID()); 
}
