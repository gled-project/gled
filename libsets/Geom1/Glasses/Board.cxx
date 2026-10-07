// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "Board.h"
#include <Glasses/ZImage.h>

using namespace gled;

#include "Board.c7"

/**************************************************************************/

void Board::_init()
{
  mTexX0 = mTexY0 = 0;
  mTexX1 = mTexY1 = 1;

  mULen  = mVLen  = 1;
  mUDivs = mVDivs = 0;
}

/**************************************************************************/
