// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "Tringula_GL_Rnr.h"

#include <Glasses/TriMesh.h>
#include <Rnr/GL/TringTvor_GL_Rnr.h>

#include <Opcode/Opcode.h>

#include <GL/glew.h>

using namespace gled;


#define PARENT ZNode_GL_Rnr

/**************************************************************************/

void Tringula_GL_Rnr::_init()
{
  mMeshTringStamp = 0;
}

Tringula_GL_Rnr::~Tringula_GL_Rnr()
{}

/******************************************************************************/

void Tringula_GL_Rnr::Draw(RnrDriver* rd)
{
  Tringula&   T = *mTringula;
  TringTvor* TT =  T.mMesh->GetTTvor();
  if (TT == 0) return;
  if (mMeshTringStamp < T.mMesh->GetStampReqTring())
  {
    bRebuildDL = true;
    mMeshTringStamp = T.mMesh->GetStampReqTring();
  }
  PARENT::Draw(rd);
}

void Tringula_GL_Rnr::Render(RnrDriver* rd)
{
  Tringula  &T  = *mTringula;
  TringTvor &TT = *T.mMesh->GetTTvor();
  glColor4fv(T.mColor());

  assert(TT.HasNorms());
  TringTvor_GL_Rnr::Render(&TT, true);
}
