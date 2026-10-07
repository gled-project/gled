// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

//__________________________________________________________________________
// MetaViewInfo
//
//

#include "MetaViewInfo.h"

using namespace gled;

#include "MetaViewInfo.c7"

/**************************************************************************/

void MetaViewInfo::_init()
{
  // *** Set all links to 0 ***
  mW = 32; mH = 8;
  bExpertP = false;
}

/**************************************************************************/

void MetaViewInfo::Size(int w, int h)
{
  mW = w; mH = h;
  Stamp(FID());
}

/**************************************************************************/
