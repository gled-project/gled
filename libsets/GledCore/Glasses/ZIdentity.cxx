// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ZIdentity.h"
#include <Glasses/ZHashList.h>
#include <Glasses/ZQueen.h>
#include <Glasses/ZMirEmittingEntity.h>

using namespace gled;

#include "ZIdentity.c7"

//__________________________________________________________________________
//
// A glass representation of a user identity. ZGroupIdentity
// represents user groups and/or virtual organizations.

/**************************************************************************/

void ZIdentity::_init()
{
  mGlassBits |= kFixedNameBit;
  mActiveMEEs = 0;
  mAllowThis = 0;
}

/**************************************************************************/

void ZIdentity::AdEnlightenment()
{
  PARENT_GLASS::AdEnlightenment();
  if(mActiveMEEs == 0) {
    assign_link<ZHashList>(mActiveMEEs, FID(), "ActiveMEEs",
			   GForm("ActiveMEEs of %s", GetName()));
    mActiveMEEs->SetElementFID(ZMirEmittingEntity::FID());
  }
}

/**************************************************************************/
