// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "MonopodialTree_GL_Rnr.h"
#include <GL/glew.h>

using namespace gled;

//==============================================================================

void MonopodialTree_GL_Rnr::_init()
{}

MonopodialTree_GL_Rnr::MonopodialTree_GL_Rnr(MonopodialTree* idol) :
  ParametricSystem_GL_Rnr(idol),
  mMonopodialTree(idol)
{
  _init();
}

MonopodialTree_GL_Rnr::~MonopodialTree_GL_Rnr()
{}

//==============================================================================
/*
void MonopodialTree_GL_Rnr::PreDraw(RnrDriver* rd) {}

void MonopodialTree_GL_Rnr::Draw(RnrDriver* rd) {}

void MonopodialTree_GL_Rnr::PostDraw(RnrDriver* rd) {}
*/

void MonopodialTree_GL_Rnr::Triangulate(RnrDriver* rd) 
{
  mMonopodialTree->Produce();
}
