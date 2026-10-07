// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ZSubTree.h"

using namespace gled;

#include "ZSubTree.c7"

void ZSubTree::_init()
{
  mRoot = 0;
  mDepth = 1; bFollowLinks = true; bFollowLists = true;
}

/**************************************************************************/
