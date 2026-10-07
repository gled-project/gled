// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "Plant.h"
using namespace gled;
#include "Plant.c7"

// Plant

//______________________________________________________________________________
//
//

//==============================================================================

void Plant::_init()
{}

Plant::Plant(const Text_t* n, const Text_t* t) :
Weed(n, t),
mStemWidth(0.015),
mLeafSize(0.5),
mFlowerSize(0.4)
{
  _init();
}

Plant::~Plant()
{}

//==============================================================================
