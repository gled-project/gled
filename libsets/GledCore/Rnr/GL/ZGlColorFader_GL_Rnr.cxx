// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ZGlColorFader_GL_Rnr.h"
#include "GLRnrDriver.h"
#include <GL/glew.h>

using namespace gled;


#define PARENT ZRnrModBase_GL_Rnr

/**************************************************************************/

void ZGlColorFader_GL_Rnr::_init()
{}

/**************************************************************************/

void ZGlColorFader_GL_Rnr::PreDraw(RnrDriver* rd)
{
  PARENT::PreDraw(rd);
  update_tring_stamp(rd);
  rd->PushRnrMod(ZGlColorFader::FID(), mRnrMod);
}

void ZGlColorFader_GL_Rnr::Draw(RnrDriver* rd)
{
  update_tring_stamp(rd);
  rd->SetDefRnrMod(ZGlColorFader::FID(), mRnrMod);
}

void ZGlColorFader_GL_Rnr::PostDraw(RnrDriver* rd)
{
  rd->PopRnrMod(ZGlColorFader::FID());
  PARENT::PostDraw(rd);
}

