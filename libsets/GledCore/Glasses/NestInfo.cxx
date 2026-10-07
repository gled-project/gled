// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

//__________________________________________________________________________
// NestInfo
//
//

#include "NestInfo.h"

#include <Ephra/Saturn.h>
#include <Glasses/ZGod.h>
#include <Glasses/ZQueen.h>

using namespace gled;

#include "NestInfo.c7"

/**************************************************************************/

const Text_t* NestInfo::sLayoutPath = "Etc/NestLayouts";

void NestInfo::_init()
{
  // Override from SubShellInfo:
  mCtorLibset = "GledCore";
  mCtorName   = "FTW_Nest";

  bShowSelf   = false;
  mMaxChildExp   = 1;

  mWName   = 30; mWAnt    = 0;
  mWIndent = 2;  mWSepBox = 1;

  mLayoutList = 0;
  mLeafLayout = LL_Ants;
}

/**************************************************************************/

void NestInfo::ImportLayout(ZGlass* src)
{
  ZList* lsrc = dynamic_cast<ZList*>(src);
  if(lsrc != 0) {
    lStr_t     parts;
    lpZGlass_t l; lsrc->CopyList(l);
    for(lpZGlass_i i=l.begin(); i!=l.end(); ++i)
      parts.push_back((*i)->GetTitle());
    mLayout = GledNS::join_strings(" : ", parts);
    mLeafLayout = LL_Custom;
  } else {
    mLayout     = src->GetTitle();
    mLeafLayout = LL_Custom;
  }
  Stamp(FID());
  EmitLayoutRay();
}

/**************************************************************************/

void NestInfo::ImportKings()
{
  lpZGlass_t kings; mSaturn->GetGod()->CopyList(kings);
  for(lpZGlass_i k=kings.begin(); k!=kings.end(); ++k)
    Add(*k);
}

/**************************************************************************/

void NestInfo::EmitLayoutRay()
{
  if(mQueen && mSaturn->AcceptsRays()) {
    std::unique_ptr<Ray> ray
      (Ray::PtrCtor(this, RayNS::RQN_user_1, mTimeStamp, FID()));
    mQueen->EmitRay(ray);
  }
}

void NestInfo::EmitRewidthRay()
{
  if(mQueen && mSaturn->AcceptsRays()) {
    std::unique_ptr<Ray> ray
      (Ray::PtrCtor(this, PRQN_rewidth, mTimeStamp, FID()));
    mQueen->EmitRay(ray);
  }
}
