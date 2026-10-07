// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

//__________________________________________________________________________
// ZRlFont
//
//

#include "ZRlFont.h"
#include <Glasses/ZQueen.h>

#include "TSystem.h"

using namespace gled;

#include "ZRlFont.c7"

/**************************************************************************/

void ZRlFont::_init()
{
  mMode = FM_Texture;
  mFontFile = GForm("%s/fonts/arial.ttf", gSystem->Getenv("ROOTSYS"));
  mSize = 16;
  mDepthFac = 0.2;
}

/**************************************************************************/

void ZRlFont::SetFontFile(const Text_t* f)
{
  mFontFile = f;
  gSystem->ExpandPathName(mFontFile);
  StampReqTring(FID());
  EmitFontChangeRay();
}

void ZRlFont::EmitFontChangeRay()
{
  if (mQueen && mSaturn->AcceptsRays())
  {
    std::unique_ptr<Ray> ray
      (Ray::PtrCtor(this, PRQN_font_change, mTimeStamp, FID()));
    mQueen->EmitRay(ray);
  }
}

void ZRlFont::EmitSizeChangeRay()
{
  if (mQueen && mSaturn->AcceptsRays())
  {
    std::unique_ptr<Ray> ray
      (Ray::PtrCtor(this, PRQN_size_change, mTimeStamp, FID()));
    mQueen->EmitRay(ray);
  }
}

void ZRlFont::EmitDepthChangeRay()
{
  if (mQueen && mSaturn->AcceptsRays())
  {
    std::unique_ptr<Ray> ray
      (Ray::PtrCtor(this, PRQN_depth_change, mTimeStamp, FID()));
    mQueen->EmitRay(ray);
  }
}
