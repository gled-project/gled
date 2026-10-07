// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "Explosion.h"
#include "Tringula.h"

using namespace gled;

#include "Explosion.c7"

// Explosion

//______________________________________________________________________________
//
//

//==============================================================================

void Explosion::_init()
{
  mTringula = 0;

  mExplodeTime = 0;
  mExplodeDuration = 1;
}

Explosion::Explosion(const Text_t* n, const Text_t* t) :
  ZGlass(n, t)
{
  _init();
}

Explosion::~Explosion()
{}

//==============================================================================

void Explosion::SetTringula(Tringula* tring)
{
  // Set tringula to which the explosion is attached.
  // Sub-classes override this to reinitialize cached data.

  mTringula = tring;
}

//==============================================================================

void Explosion::TimeTick(Double_t t, Double_t dt)
{
  mExplodeTime += dt;
  if (mExplodeTime > mExplodeDuration)
  {
    mTringula->ExplosionFinished(this);
  }
}
