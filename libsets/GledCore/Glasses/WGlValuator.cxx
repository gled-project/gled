// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

//__________________________________________________________________________
// WGlValuator
//
// In principle should install Alpha-Observer ... but pupils are ROARs sofar.

#include "WGlValuator.h"

#include <TClass.h>
#include <TRealData.h>

using namespace gled;

#include "WGlValuator.c7"

/**************************************************************************/

void WGlValuator::_init()
{
  mMin   = -1000; mMax   = 1000;
  mStepA =  1;    mStepB = 1;
  mFormat = "%6.2f";

  bConstVal = false;

  mDataMemberInfo = 0; mDataMember = 0;
}

/**************************************************************************/

GledNS::DataMemberInfo* WGlValuator::GetDataMemberInfo()
{
  if(mDataMemberInfo == 0)
    mDataMemberInfo = GledNS::DeduceDataMemberInfo(*mCbackAlpha, mCbackMemberName.Data());
  return mDataMemberInfo;
}

TDataMember* WGlValuator::GetDataMember()
{
  if(mDataMember == 0 && mCbackAlpha != 0) {
    if(GetDataMemberInfo() == 0)
      return 0;
    mDataMember = mDataMemberInfo->GetTDataMember();
  }
  return mDataMember;
}

Bool_t WGlValuator::DataOK()
{
  return (GetDataMemberInfo() != 0 && GetDataMember() != 0);
}

/**************************************************************************/
