// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

//__________________________________________________________________________
// ZIdentityListFilter
//
//

#include "ZIdentityListFilter.h"
#include <Glasses/ZHashList.h>
#include "Glasses/ZIdentity.h"
#include <Glasses/ZMirEmittingEntity.h>

using namespace gled;

#include "ZIdentityListFilter.c7"

/**************************************************************************/

void ZIdentityListFilter::_init()
{
  mOnMatch = ZMirFilter::R_Allow;
}

/**************************************************************************/

ZMirFilter::Result_e ZIdentityListFilter::FilterMIR(ZMIR& mir)
{
  if(mIdentities != 0) {
    GMutexHolder ids_lock(mIdentities->RefListMutex());
    AList::Stepper<> s(*mIdentities);
    while(s.step()) {
      if(mir.fCaller->HasIdentity((ZIdentity*)*s))
	return (Result_e)mOnMatch;
    }
    return NegateResult((Result_e)mOnMatch);
  }
  return PARENT_GLASS::FilterMIR(mir);
}

/**************************************************************************/
