// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "Explosion_GL_Rnr.h"
#include <GL/glew.h>

using namespace gled;


//==============================================================================

void Explosion_GL_Rnr::_init()
{}

Explosion_GL_Rnr::Explosion_GL_Rnr(Explosion* idol) :
  ZGlass_GL_Rnr(idol),
  mExplosion(idol)
{
  _init();
}

Explosion_GL_Rnr::~Explosion_GL_Rnr()
{}

//==============================================================================

// void Explosion_GL_Rnr::PreDraw(RnrDriver* rd) {}

// void Explosion_GL_Rnr::Draw(RnrDriver* rd) {}

// void Explosion_GL_Rnr::PostDraw(RnrDriver* rd) {}

// void Explosion_GL_Rnr::Render(RnrDriver* rd) {}
