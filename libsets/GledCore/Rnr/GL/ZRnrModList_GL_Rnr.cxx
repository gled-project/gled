// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ZRnrModList_GL_Rnr.h"
#include <RnrBase/RnrDriver.h>
#include <GL/glew.h>

using namespace gled;


namespace OS = OptoStructs;

#define PARENT ZGlass_GL_Rnr

/**************************************************************************/

void ZRnrModList_GL_Rnr::_init()
{}

/**************************************************************************/

// Could further optimize with two rnr-schemes for pre/post draw.
// Also need AbsorbRay to destroy them on list-change.

void ZRnrModList_GL_Rnr::PreDraw(RnrDriver* rd)
{
  PARENT::PreDraw(rd);
  OS::lpZGlassImg_t* imgs = fImg->GetElementImgs();
  for(OS::lpZGlassImg_i img=imgs->begin(); img!=imgs->end(); ++img)
    rd->GetRnr(*img)->PreDraw(rd);
}

void ZRnrModList_GL_Rnr::PostDraw(RnrDriver* rd)
{
  OS::lpZGlassImg_t* imgs = fImg->GetElementImgs();
  for(OS::lpZGlassImg_ri img=imgs->rbegin(); img!=imgs->rend(); ++img)
    rd->GetRnr(*img)->PostDraw(rd);
  PARENT::PostDraw(rd);
}
