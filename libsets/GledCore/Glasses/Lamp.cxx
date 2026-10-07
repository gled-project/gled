// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "Lamp.h"

using namespace gled;

#include "Lamp.c7"

/**************************************************************************/

void Lamp::_init()
{
  mAmbient.rgba(0.05, 0.05, 0.05, 1.0);
  mDiffuse.rgba(1, 1, 1, 1);
  mSpecular.rgba(0.2, 0.2, 0.2, 1.0);
  mLampScale = 0;
  mSpotExp = 0; mSpotCutOff = 180;
  mConstAtt = 1; mLinAtt = mQuadAtt = 0;

  bDrawLamp = true; bOnIfOff = false; bOffIfOn = false;
}

/**************************************************************************/
