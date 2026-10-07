// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

//__________________________________________________________________________
// AlListener
//
//

#include "AlListener.h"

#include <AL/alut.h>

using namespace gled;

#include "AlListener.c7"

/**************************************************************************/

void AlListener::_init()
{
  mLocationType = LT_Camera;
  mGain = 1;
}

/**************************************************************************/

void AlListener::EmitSourceRay()
{
  alListenerf(AL_GAIN, mGain);
}
