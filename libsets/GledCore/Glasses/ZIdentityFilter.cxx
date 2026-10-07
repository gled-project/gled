// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ZIdentityFilter.h"
#include <Glasses/ZIdentity.h>
#include <Glasses/ZMirEmittingEntity.h>

using namespace gled;

#include "ZIdentityFilter.c7"

//__________________________________________________________________________
// ZIdentityFilter
//
//

/**************************************************************************/

void ZIdentityFilter::_init()
{
  mIdentity = 0;
  mOnMatch = ZMirFilter::R_Allow;
}

/**************************************************************************/

ZMirFilter::Result_e ZIdentityFilter::FilterMIR(ZMIR& mir)
{
  if(mIdentity != 0) {
    if(mir.fCaller->HasIdentity(mIdentity.get()))
      return (Result_e)mOnMatch;
    else
      return NegateResult((Result_e)mOnMatch);
  }
  return PARENT_GLASS::FilterMIR(mir);
}


/**************************************************************************/
