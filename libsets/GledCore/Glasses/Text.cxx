// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

//__________________________________________________________________________
// Text
//
//

#include "Text.h"

using namespace gled;

#include "Text.c7"

/**************************************************************************/

void Text::_init()
{
  mFont = 0;

  // Override settings from ZGlass
  bUseDispList = true;
  // Override settings from ZNode
  bUseScale    = true;

  bAlpha     = false;   bBlend     = true;
  bAbsSize   = true;    bCenter    = true;
  bBackPoly  = true;
  bFramePoly = true;
  bFillBack  = false;
  mXBorder   = 0.1;     mYBorder   = 0.1;
  mFGCol.rgba(1,1,1,1); mBGCol.rgba(0,0,0,1);
  mFrameW    = 0.05;
  mFrameCol.gray(0.8);
}

/**************************************************************************/
