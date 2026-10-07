// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

//__________________________________________________________________________
// Box
//
//

#include "Box.h"

using namespace gled;

#include "Box.c7"

/**************************************************************************/

void Box::_init()
{
  mA = mB = mC = 1;
}

/**************************************************************************/

void Box::SetABC(Float_t a, Float_t b, Float_t c)
{
  mA = a; mB = b; mC = c;
  Stamp(FID());
}
