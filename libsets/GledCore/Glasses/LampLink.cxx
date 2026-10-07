// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

//__________________________________________________________________________
// LampLink
//
//

#include "LampLink.h"

using namespace gled;

#include "LampLink.c7"

/**************************************************************************/

void LampLink::_init()
{
  mLamp = 0;
  bTurnOn = true; bTurnOff = false;
}

/**************************************************************************/

