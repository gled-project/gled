// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

//__________________________________________________________________________
// ZRlNodeMarkup
//
//

#include "ZRlNodeMarkup.h"

using namespace gled;

#include "ZRlNodeMarkup.c7"

/**************************************************************************/

void ZRlNodeMarkup::_init()
{
  mNodeMarkupOp  = O_On;

  bRnrAxes    = false;
  mAxeWidth   = 0;  mAxeLength = 1.2;

  bRnrNames   = true;
  bRnrTiles   = true;  bRnrFrames = true;
  mNameOffset = 0.99;
  mTextCol.rgba(1, 1, 1);
  mTileCol.rgba(0, 0, 0.3);
  mTilePos = "";
}

/**************************************************************************/
