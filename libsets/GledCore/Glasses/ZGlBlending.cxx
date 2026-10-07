// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

//__________________________________________________________________________
// ZGlBlending
//
//

#include "ZGlBlending.h"

#include <GL/glew.h>

using namespace gled;

#include "ZGlBlending.c7"

/**************************************************************************/

void ZGlBlending::_init()
{
  mBlendOp   = O_Nop;
  mBSrcFac   = GL_SRC_ALPHA;
  mBDstFac   = GL_ONE_MINUS_SRC_ALPHA;
  mBEquation = GL_FUNC_ADD;
  mBConstCol.rgba(0.5, 0.5, 0.5, 0.5);

  mAntiAliasOp = O_Nop;
  bPointSmooth = true;
  mPointSize   = 1;
  mPointHint   = GL_FASTEST;
  bLineSmooth  = true;
  mLineWidth   = 1;
  mLineHint    = GL_FASTEST;

  mFogOp   = O_Nop;
  mFogMode = GL_EXP;
  mFogHint = GL_FASTEST;
  mFogColor.rgba(0.1,0.1,0.1);
  mFogDensity = 0.2;
  mFogBeg     = 0;
  mFogEnd     = 20;

  mDepthOp   = O_Nop;
  mDepthFunc = GL_LESS;
  mDepthMaskOp = O_Nop;
}

/**************************************************************************/


/**************************************************************************/
