// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

//__________________________________________________________________________
// ZEunuch
//
//

#include "ZEunuch.h"

using namespace gled;

#include "ZEunuch.c7"

/**************************************************************************/

void ZEunuch::_init()
{
  mPrimQueen = 0;
  mSecQueen  = 0;
  mToSaturn = 0;

  mRequest = RT_Undef;
  mPushStrategy = PS_Undef;
}

/**************************************************************************/


/**************************************************************************/
