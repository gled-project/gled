// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

//__________________________________________________________________________
// MetaSubViewInfo
//
//

#include "MetaSubViewInfo.h"

using namespace gled;

#include "MetaSubViewInfo.c7"

/**************************************************************************/

void MetaSubViewInfo::_init()
{
  // *** Set all links to 0 ***
  mX = mY = 0;
}

/**************************************************************************/

void MetaSubViewInfo::Position(int x, int y)
{
  mX = x; mY = y;
  Stamp(FID());
}

/**************************************************************************/
