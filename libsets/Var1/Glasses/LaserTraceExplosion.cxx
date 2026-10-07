// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "LaserTraceExplosion.h"

using namespace gled;

#include "LaserTraceExplosion.c7"

// LaserTraceExplosion

//______________________________________________________________________________
//
//

//==============================================================================

LaserTraceExplosion::LaserTraceExplosion(const Text_t* n, const Text_t* t) :
  Explosion(n, t),
  mEndRadius(0)
{
  // From ZGlass.
  bUseDispList  = true;
}

LaserTraceExplosion::~LaserTraceExplosion()
{}
