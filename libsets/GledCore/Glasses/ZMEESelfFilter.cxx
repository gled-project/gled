// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

//__________________________________________________________________________
// ZMEESelfFilter
//
//

#include "ZMEESelfFilter.h"
#include <Glasses/ZMirEmittingEntity.h>

using namespace gled;

#include "ZMEESelfFilter.c7"

/**************************************************************************/

void ZMEESelfFilter::_init()
{
}

/**************************************************************************/

ZMirFilter::Result_e ZMEESelfFilter::FilterMIR(ZMIR& mir)
{
  // Allows execution if Alpha == Caller (which is a MEE) and
  // the method is tagged by the "MEE::Self" tag.
  // Otherwise returns a remapped R_None.

  const TString mee_self_tag("MEE::Self");

  if (mir.fCaller == mir.fAlpha)
  {
    GledNS::ClassInfo  *ci = GledNS::FindClassInfo(FID_t(mir.fLid, mir.fCid));
    GledNS::MethodInfo *mi = ci->FindMethodInfo(mir.fMid);
    for (lStr_i i=mi->fTags.begin(); i!=mi->fTags.end(); ++i)
    {
      if (*i == mee_self_tag)
	return ZMirFilter::R_Allow;
    }
  }
  return PARENT_GLASS::FilterMIR(mir);
}
