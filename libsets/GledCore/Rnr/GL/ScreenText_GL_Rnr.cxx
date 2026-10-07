// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ScreenText_GL_Rnr.h"
#include <Rnr/GL/GLTextNS.h>
#include <GL/glew.h>

using namespace gled;


/**************************************************************************/

void ScreenText_GL_Rnr::_init()
{}

/**************************************************************************/

//void ScreenText_GL_Rnr::PreDraw(RnrDriver* rd)
//{}

void ScreenText_GL_Rnr::Draw(RnrDriver* rd)
{
  ScreenText& T = *mScreenText;
  GLTextNS::RnrTextAt(rd, T.RefText(), T.mX, T.mY, T.mZ, T.PtrFgCol(), T.PtrBgCol());
}

//void ScreenText_GL_Rnr::PostDraw(RnrDriver* rd)
//{}
