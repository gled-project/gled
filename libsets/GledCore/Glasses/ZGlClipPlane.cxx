// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

//__________________________________________________________________________
// ZGlClipPlane
//
//

#include "ZGlClipPlane.h"

using namespace gled;

#include "ZGlClipPlane.c7"

/**************************************************************************/

void ZGlClipPlane::_init()
{
  mX = mY = mZ = 0;
  mDist = mTheta = mPhi = 0;

  bRnrSelf = true; bOnIfOff = true; bOffIfOn = false;
}

/**************************************************************************/


/**************************************************************************/
