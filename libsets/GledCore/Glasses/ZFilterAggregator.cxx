// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

//__________________________________________________________________________
// ZFilterAggregator
//
//

#include "ZFilterAggregator.h"

using namespace gled;

#include "ZFilterAggregator.c7"

/**************************************************************************/

void ZFilterAggregator::_init()
{
  bStrongNone = bPreemptNone = false;
  bPreemptAllow = true;
  bPreemptDeny  = false;
  mFilters = 0;
}

/**************************************************************************/

ZMirFilter::Result_e ZFilterAggregator::FilterMIR(ZMIR& mir)
{
  if(mFilters != 0) {
    UChar_t result  = 0;
    UChar_t preempt = BuildPreemptionBits();
    {
      GMutexHolder filter_lock(mFilters->RefListMutex());
      AList::Stepper<> s(*mFilters);
      while(s.step()) {
	UChar_t r = ((ZMirFilter*)*s)->FilterMIR(mir);
	if(r & preempt) {
	  return FinaliseResult((Result_e)r);
	}
	result |= r;
	if(result & R_Allow && result & R_Deny) break;
      }
    }
    if( (result & R_Allow && result & R_Deny) ||
        (result & ZMirFilter::R_None && bStrongNone) )
      {
	return FinaliseResult(R_None);
      }
    if(result & R_Allow) return R_Allow;
    if(result & R_Deny)  return R_Deny;
  }
  return PARENT_GLASS::FilterMIR(mir);
}


/**************************************************************************/
