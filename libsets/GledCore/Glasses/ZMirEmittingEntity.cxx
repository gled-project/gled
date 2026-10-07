// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ZMirEmittingEntity.h"
#include <Glasses/ZQueen.h>

using namespace gled;

#include "ZMirEmittingEntity.c7"

//__________________________________________________________________________
// ZMirEmittingEntity
//
//

/**************************************************************************/

void ZMirEmittingEntity::_init()
{
  mPrimaryIdentity  = 0;
  mActiveIdentities = 0;
}

/**************************************************************************/

void ZMirEmittingEntity::AdEnlightenment()
{
  PARENT_GLASS::AdEnlightenment();
  if(mActiveIdentities == 0) {
    assign_link<ZHashList>(mActiveIdentities, FID(), "ActiveIdentities",
                           GForm("ActiveIdentities of %s", GetName()));
    mActiveIdentities->SetElementFID(ZIdentity::FID());
  }
}

/**************************************************************************/

Bool_t ZMirEmittingEntity::HasIdentity(ZIdentity* ident)
{
  return ( mPrimaryIdentity == ident ||
	  (mActiveIdentities != 0 && mActiveIdentities->Has(ident))
	 );
}

/**************************************************************************/
