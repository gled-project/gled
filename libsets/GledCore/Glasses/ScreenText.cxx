// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

//__________________________________________________________________________
// ScreenText
//
// Displays mText in window coordinates. This class only contains the
// instructions, actual work is done by renderers.

#include "ScreenText.h"

using namespace gled;

#include "ScreenText.c7"

/**************************************************************************/

void ScreenText::_init()
{
  mBgCol.rgba(0,0,0,0.5);
  mX = 0; mY = 0; mZ = 1e-3;
}
