// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "Rect.h"

using namespace gled;

#include "Rect.c7"

void Rect::_init()
{
  mULen = mVLen = 1;
  mUStrips = mVStrips = 10;
  mWidth = 1;
}
