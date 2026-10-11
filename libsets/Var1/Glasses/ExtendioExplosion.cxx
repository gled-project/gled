// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ExtendioExplosion.h"
#include "Extendio.h"

#include "Tringula.h"
#include "TriMesh.h"

#include <algorithm>

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
  std::fill_n(mCtrExtBox, 6, 0.0f);
}

ExtendioExplosion::~ExtendioExplosion()
{}

//==============================================================================

void ExtendioExplosion::SetExtendio(Extendio* ext)
{
  mExtendio = ext;
  const Float_t *bb = ext->get_tring_tvor()->mCtrExtBox;
  std::copy_n(bb, 6, mCtrExtBox);
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
