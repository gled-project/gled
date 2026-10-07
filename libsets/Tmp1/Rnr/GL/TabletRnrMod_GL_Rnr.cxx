// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "TabletRnrMod_GL_Rnr.h"
#include <Rnr/GL/GLRnrDriver.h>

using namespace gled;

// #include <GL/glew.h>

#define PARENT ZRnrModBase_GL_Rnr

//==============================================================================

void TabletRnrMod_GL_Rnr::_init()
{}

TabletRnrMod_GL_Rnr::TabletRnrMod_GL_Rnr(TabletRnrMod* idol) :
  ZRnrModBase_GL_Rnr(idol),
  mTabletRnrMod(idol)
{
  _init();
}

TabletRnrMod_GL_Rnr::~TabletRnrMod_GL_Rnr()
{}

//==============================================================================

void TabletRnrMod_GL_Rnr::PreDraw(RnrDriver* rd)
{
  PARENT::PreDraw(rd);
  update_tring_stamp(rd);
  rd->PushRnrMod(TabletRnrMod::FID(), mRnrMod);
}

void TabletRnrMod_GL_Rnr::Draw(RnrDriver* rd)
{
  update_tring_stamp(rd);
  rd->SetDefRnrMod(TabletRnrMod::FID(), mRnrMod);
}

void TabletRnrMod_GL_Rnr::PostDraw(RnrDriver* rd)
{
  rd->PopRnrMod(TabletRnrMod::FID());
  PARENT::PostDraw(rd);
}
