// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "TabletRnrMod.h"
using namespace gled;
#include "TabletRnrMod.c7"

// TabletRnrMod

//______________________________________________________________________________
//
//

//==============================================================================

void TabletRnrMod::_init()
{
  mMarkSize = 0.05;
  mPressCurveAlpha = 1;

  mInTouchColor.rgba(1, 0, 0, 1);
  mInProximityColor.rgba(0, 1, 0, 1);

  mPointColor.rgba(1, 0.5, 0, 1);
  mLineColor .rgba(0, 0.5, 1, 1);
  bRnrPoints =  true;
  bRnrSpheres = false;
}

TabletRnrMod::TabletRnrMod(const Text_t* n, const Text_t* t) :
  ZRnrModBase(n, t)
{
  _init();
}

TabletRnrMod::~TabletRnrMod()
{}

//==============================================================================
