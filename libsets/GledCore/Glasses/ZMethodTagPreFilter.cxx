// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

//__________________________________________________________________________
// ZMethodTagPreFilter
//
// Contains a comma-separated list of tags and a link to ZMirFilter.
//

#include "ZMethodTagPreFilter.h"

using namespace gled;

#include "ZMethodTagPreFilter.c7"

/**************************************************************************/

void ZMethodTagPreFilter::_init()
{
  mFilter = 0;
}

/**************************************************************************/
ZMirFilter::Result_e ZMethodTagPreFilter::FilterMIR(ZMIR& mir)
{
  // If Method being called contains one of the tags in mTags, then
  // the result of mFilter->FilterMIR(mir) is returned.
  //
  // This is written in a terrribly inefficient manner.

  lStr_t tags;
  int cnt = GledNS::split_string(mTags.Data(), tags, ',');
  if(cnt) {
    GledNS::ClassInfo* ci = GledNS::FindClassInfo(FID_t(mir.fLid, mir.fCid));
    GledNS::MethodInfo* mi = ci->FindMethodInfo(mir.fMid);
    for(lStr_i i=tags.begin(); i!=tags.end(); ++i) {
      for(lStr_i j=mi->fTags.begin(); j!=mi->fTags.end(); ++j) {
	if(*i == *j) {
	  if(mFilter != 0)
	    return mFilter->FilterMIR(mir);
	  else
	    return PARENT_GLASS::FilterMIR(mir);
	}
      }
    }
  }

  return R_None;
}

/**************************************************************************/
