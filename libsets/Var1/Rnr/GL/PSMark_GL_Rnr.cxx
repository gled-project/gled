// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "PSMark_GL_Rnr.h"
#include <GL/glew.h>

using namespace gled;


/**************************************************************************/

void PSMark_GL_Rnr::_init()
{}

/**************************************************************************/

void PSMark_GL_Rnr::Render(RnrDriver* rd)
{
  GL_Capability_Switch loff(GL_LIGHTING, false);

  // draw point / sphere ??
  glBegin(GL_LINES);
  glColor3f(1, 0, 0); glVertex3f(0, 0, 0); glVertex3f(1, 0, 0);
  glColor3f(0, 1, 0); glVertex3f(0, 0, 0); glVertex3f(0, 1, 0);
  glColor3f(0, 0, 1); glVertex3f(0, 0, 0); glVertex3f(0, 0, 1);
  glEnd();
}
