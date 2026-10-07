// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

//__________________________________________________________________________
// ZGlLightModel
//
//

#include "ZGlLightModel.h"

#include <GL/glew.h>

using namespace gled;

#include "ZGlLightModel.c7"

/**************************************************************************/

void ZGlLightModel::_init()
{
  mLightModelOp   = O_Nop;
  mLiMoAmbient.rgba(0.2, 0.2, 0.2);
  mLiMoColorCtrl = GL_SINGLE_COLOR;
  bLiMoLocViewer = false;
  bLiMoTwoSide   = false;

  mShadeModelOp = O_Nop;
  mShadeModel   = GL_SMOOTH;
  mFrontFace    = GL_CCW;
  mFrontMode    = GL_FILL;
  mBackMode     = GL_FILL;
  bDepthMask    = true;

  mFaceCullOp   = O_Nop;
  mFaceCullMode = GL_BACK;
}

/**************************************************************************/
