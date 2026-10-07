// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ExtendioExplosion.h"
#include "Extendio.h"

#include "Tringula.h"

using namespace gled;

#include "ExtendioExplosion.c7"

// ExtendioExplosion

//______________________________________________________________________________
//
//

//==============================================================================

ExtendioExplosion::ExtendioExplosion(const Text_t* n, const Text_t* t) :
  Explosion(n, t)
{
  // From ZGlass.
  bUseDispList  = true;

  mExtendio = 0;
}

ExtendioExplosion::~ExtendioExplosion()
{}

//==============================================================================

void ExtendioExplosion::SetExtendio(Extendio* ext)
{
  mExtendio = ext;
}

//==============================================================================

void ExtendioExplosion::TimeTick(Double_t t, Double_t dt)
{
  mExplodeTime += dt;
  if (mExplodeTime > mExplodeDuration)
  {
    mTringula->ExtendioExplosionFinished(this);
  }
}
