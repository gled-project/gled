// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

//__________________________________________________________________________
// ZGlMaterial
//
//

#include "ZGlMaterial.h"

#include <GL/glew.h>

using namespace gled;

#include "ZGlMaterial.c7"

/**************************************************************************/

void ZGlMaterial::_init()
{
  mMatOp     = O_On;
  mFace      = GL_FRONT_AND_BACK;
  mShininess = 64;
  mAmbient.rgba(0.2,0.2,0.2);
  mDiffuse.rgba(0.8,0.8,0.8);
  mSpecular.rgba(0,0,0);
  mEmission.rgba(0,0,0);

  mModeOp    = O_Nop;
  mModeFace  = GL_FRONT_AND_BACK;
  mModeColor = GL_AMBIENT_AND_DIFFUSE;
}

/**************************************************************************/


/**************************************************************************/
