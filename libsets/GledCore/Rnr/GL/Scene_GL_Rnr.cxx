// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "Scene_GL_Rnr.h"
#include <GL/glew.h>

using namespace gled;


/**************************************************************************/

void Scene_GL_Rnr::_init()
{
  // From ZGlass_GL_Rnr:
  bSuppressNameLabel = true;
}

/**************************************************************************/

// In principle Scene could handle global lamps.
// Now there's a GlobalLamps glass with its own renderer.

/*
void Scene_GL_Rnr::PreDraw(RnrDriver* rd)
{

}
*/

/*
void Scene_GL_Rnr::Draw(RnrDriver* rd)
{
  printf("rendering scene %s\n", mScene->GetName());
  ZNode_GL_Rnr::Draw(rd);
}
*/

/*
void Scene_GL_Rnr::PostDraw(RnrDriver* rd)
{

}
*/
