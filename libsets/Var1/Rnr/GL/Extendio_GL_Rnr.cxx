// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "Extendio_GL_Rnr.h"

#include <Glasses/Tringula.h>

#include <Rnr/GL/TringTvor_GL_Rnr.h>
#include <Rnr/GL/GLRnrDriver.h>

#include <GL/glew.h>

using namespace gled;


#define PARENT ZGlass_GL_Rnr

/**************************************************************************/

void Extendio_GL_Rnr::_init()
{
  bRnrBBoxes = false;
}

Extendio_GL_Rnr::Extendio_GL_Rnr(Extendio* idol) :
    ZGlass_GL_Rnr(idol), mExtendio(idol)
{
  _init();
}

Extendio_GL_Rnr::~Extendio_GL_Rnr()
{}

/**************************************************************************/

bool Extendio_GL_Rnr::GetCullBox(RnrDriver* rd, Float_t* ctr_ext)
{
  if (mExtendio->GetMesh() == 0) return false;
  Opcode::Point c, e;
  mExtendio->RefLastAABB().GetCenter(c);
  mExtendio->RefLastAABB().GetExtents(e);
  ctr_ext[0] = c.x; ctr_ext[1] = c.y; ctr_ext[2] = c.z;
  ctr_ext[3] = e.x; ctr_ext[4] = e.y; ctr_ext[5] = e.z;
  return true;
}

void Extendio_GL_Rnr::PreDraw(RnrDriver* rd)
{
  PARENT::PreDraw(rd);
  glPushMatrix();
  glMultMatrixf(mExtendio->RefLastTrans().Array());
}

void Extendio_GL_Rnr::Draw(RnrDriver* rd)
{
  GET_OR_RET(TriMesh, mesh, mExtendio->GetMesh());

  rd->GL()->Color(mExtendio->mColor);

  PARENT::Draw(rd);

  Extendio &E = * mExtendio;

  bRnrBBoxes = E.mTringula && E.mTringula->GetRnrBBoxes();

  if (bRnrBBoxes)
  {
    GL_Capability_Switch ligt_off(GL_LIGHTING, false);
    glColor3f(1, 0, 0);
    TringTvor_GL_Rnr::RenderCEBBox(mesh->GetTTvor()->mCtrExtBox, 1.01f);
  }
}

void Extendio_GL_Rnr::Render(RnrDriver* rd)
{
  GET_OR_RET(TriMesh, mesh, mExtendio->GetMesh());

  TringTvor_GL_Rnr::Render(mesh->GetTTvor());
}

void Extendio_GL_Rnr::PostDraw(RnrDriver* rd)
{
  glPopMatrix();

  Extendio &E = * mExtendio;

  if (bRnrBBoxes)
  {
    GL_Capability_Switch ligt_off(GL_LIGHTING, false);
    glColor3f(0, 0, 1);
    TringTvor_GL_Rnr::RenderCEBBox((Float_t*)&E.RefLastAABB(), 1.01f);
  }

  PARENT::PostDraw(rd);
}
