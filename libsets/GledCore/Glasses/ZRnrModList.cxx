// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

//__________________________________________________________________________
// ZRnrModList
//
// All functionality in Rnr class (which sub-classes ZGlass_GL_Rnr):
// on PreDraw calls PreDraw of all elements (in list order).
// Likewise on PostDraw.

#include "ZRnrModList.h"

using namespace gled;

#include "ZRnrModList.c7"

/**************************************************************************/

void ZRnrModList::_init()
{
  // From ZGlass:
  bUseNameStack = false;
}
