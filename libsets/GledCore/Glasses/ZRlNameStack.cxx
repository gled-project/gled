// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

//__________________________________________________________________________
// ZRlNameStack
//
//

#include "ZRlNameStack.h"

using namespace gled;

#include "ZRlNameStack.c7"

/**************************************************************************/

void ZRlNameStack::_init()
{
  mNameStackOp = O_Nop;
  bClearStack    = false;
  bRestoreStack  = false;
}

/**************************************************************************/


/**************************************************************************/
