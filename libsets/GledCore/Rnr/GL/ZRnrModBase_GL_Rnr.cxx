// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ZRnrModBase_GL_Rnr.h"
#include <GL/glew.h>

using namespace gled;


#define PARENT ZGlass_GL_Rnr

/**************************************************************************/

void ZRnrModBase_GL_Rnr::_init()
{
  mRnrMod = new RnrMod(mZRnrModBase, this);
}

ZRnrModBase_GL_Rnr::~ZRnrModBase_GL_Rnr()
{
  delete mRnrMod;
}

/**************************************************************************/

void ZRnrModBase_GL_Rnr::update_tring_stamp(RnrDriver* rd)
{
  // Usually called by sub-classes from PreDraw and Draw.

  mRnrMod->fTringTS = mZRnrModBase->GetStampReqTring();
}
