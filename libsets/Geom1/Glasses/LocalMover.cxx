// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

//__________________________________________________________________________
// LocalMover
//
//

#include "LocalMover.h"

#include <Stones/ZTrans.h>

using namespace gled;

#include "LocalMover.c7"

/**************************************************************************/

void LocalMover::_init()
{
  bMoveOn   = true;
  bMoveInPF = false;
  mDx = mDy = mDz = 0;

  bRotOn   = true;
  bRotInPF = false;
  mPhi = mTheta = mEta = 0;

  mRotMatrix = 0;
}

/**************************************************************************/


/**************************************************************************/
