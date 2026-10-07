// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ProductionRule.h"
using namespace gled;
#include "ProductionRule.c7"

// ProductionRule

//______________________________________________________________________________
//
//

//==============================================================================

void ProductionRule::_init()
{}

ProductionRule::ProductionRule(const Text_t* n, const Text_t* t) :
  ZGlass(n, t),
  mConsumer(0)
{
  _init();
  mRule = t;
}

ProductionRule::~ProductionRule()
{}

//==============================================================================
void ProductionRule::SetRule(const Text_t* t)
{
  mRule = t;
  mConsumer->ReTriangulate();
}
