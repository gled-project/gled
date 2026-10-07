// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "SMorph_GL_Rnr.h"
#include <Rnr/GL/GLRnrDriver.h>
#include <Rnr/GL/TubeTvor_GL_Rnr.h>

using namespace gled;


/**************************************************************************/

void SMorph_GL_Rnr::Render(RnrDriver* rd)
{
  glPushAttrib(GL_CURRENT_BIT);
  rd->GL()->Color(mSMorph->mColor);
  if(mSMorph->pTuber) TubeTvor_GL_Rnr::Render(mSMorph->pTuber);
  glPopAttrib();
}

void SMorph_GL_Rnr::Triangulate(RnrDriver* rd)
{
  ZNode_GL_Rnr::Triangulate(rd);
  mSMorph->Triangulate();
}

/**************************************************************************/
