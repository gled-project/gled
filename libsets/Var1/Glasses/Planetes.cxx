// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "Planetes.h"
#include "HTriMesh.h"

using namespace gled;

#include "Planetes.c7"

// Planetes

//______________________________________________________________________________
//
//

//==============================================================================

void Planetes::_init()
{
  // Override settings from ZGlass
  bUseDispList = true;

  mColor.rgba(1, 0.6, 0.8);
  mDrawLevel = 0;
}

Planetes::Planetes(const Text_t* n, const Text_t* t) :
  ZNode(n, t)
{
  _init();
}

Planetes::~Planetes()
{}

//==============================================================================

void Planetes::SetMesh(HTriMesh* mesh)
{
  set_link_or_die(mMesh.ref_link(), mesh, FID());
  if (*mMesh)
  {
    SetDrawLevel(mDrawLevel);
  }
}

void Planetes::SetDrawLevel(Int_t l)
{
  Int_t l_max = (*mMesh) ? mMesh->GetMaxLevel() : 0;
  if (l < 0) l = 0;
  if (l > l_max) l = l_max;
  if (l != mDrawLevel)
  {
    mDrawLevel = l;
    StampReqTring(FID());
  }
}
