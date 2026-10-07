// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ZRlNodeMarkup_GL_Rnr.h"
#include <Rnr/GL/GLRnrDriver.h>
#include <TSystem.h>
#include <GL/glew.h>

using namespace gled;


#define PARENT ZRnrModBase_GL_Rnr

/**************************************************************************/

void ZRlNodeMarkup_GL_Rnr::_init()
{}

/**************************************************************************/

void ZRlNodeMarkup_GL_Rnr::PreDraw(RnrDriver* rd)
{
  PARENT::PreDraw(rd);
  bExState = rd->GL()->GetMarkupNodes();
  switch(mZRlNodeMarkup->mNodeMarkupOp) {
  case ZRnrModBase::O_On:
    update_tring_stamp(rd);
    rd->PushRnrMod(ZRlNodeMarkup::FID(), mRnrMod);
    rd->GL()->SetMarkupNodes(true);
    break;
  case ZRnrModBase::O_Off:
    rd->GL()->SetMarkupNodes(false);
    break;
  case ZRnrModBase::O_Nop:
    break;
  }
}

void ZRlNodeMarkup_GL_Rnr::Draw(RnrDriver* rd)
{
  switch(mZRlNodeMarkup->mNodeMarkupOp) {
  case ZRnrModBase::O_On:
    update_tring_stamp(rd);
    rd->SetDefRnrMod(ZRlNodeMarkup::FID(), mRnrMod);
    rd->GL()->SetMarkupNodes(true);
    break;
  case ZRnrModBase::O_Off:
    rd->GL()->SetMarkupNodes(false);
    break;
  case ZRnrModBase::O_Nop:
    break;
  }
}

void ZRlNodeMarkup_GL_Rnr::PostDraw(RnrDriver* rd)
{
  switch(mZRlNodeMarkup->mNodeMarkupOp) {
  case ZRnrModBase::O_On:
    rd->PopRnrMod(ZRlNodeMarkup::FID());
    rd->GL()->SetMarkupNodes(bExState);
    break;
  case ZRnrModBase::O_Off:
    rd->GL()->SetMarkupNodes(bExState);
    break;
  case ZRnrModBase::O_Nop:
    break;
  }
  PARENT::PostDraw(rd);
}
