// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "@CLASS@_GL_Rnr.h"
#include <RnrBase/RnrDriver.h>

#include <GL/glew.h>

using namespace gled;

//==============================================================================

void @CLASS@_GL_Rnr::_init()
{}

@CLASS@_GL_Rnr::@CLASS@_GL_Rnr(@CLASS@* idol) :
  @BASE@_GL_Rnr(idol),
  m@CLASS@(idol)
{
  _init();
}

@CLASS@_GL_Rnr::~@CLASS@_GL_Rnr()
{}

//==============================================================================

void @CLASS@_GL_Rnr::Draw(RnrDriver* rd)
{}
