// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

//__________________________________________________________________________
// Scene
//
//

#include "Scene.h"
#include <Glasses/ZQueen.h>

using namespace gled;

#include "Scene.c7"

/**************************************************************************/

void Scene::_init()
{
  mGlobLamps = 0;
}

/**************************************************************************/

void Scene::AdEnlightenment()
{
  ZNode::AdEnlightenment();
  if(mGlobLamps == 0) {
    assign_link<GlobalLamps>(mGlobLamps, FID(), "Global Lamps", GForm("GlobLamps of %s", GetName()));
  }
}

/**************************************************************************/
