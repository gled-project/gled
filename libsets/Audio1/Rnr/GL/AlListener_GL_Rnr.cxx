// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "AlListener_GL_Rnr.h"
#include <RnrBase/RnrDriver.h>

#include <AL/alut.h>
#include <GL/glew.h>

using namespace gled;


/**************************************************************************/

void AlListener_GL_Rnr::_init()
{}

/**************************************************************************/

//void AlListener_GL_Rnr::PreDraw(RnrDriver* rd)
//{}

void AlListener_GL_Rnr::Draw(RnrDriver* rd)
{
  ZTrans* tp = 0;
  switch (mAlListener->mLocationType)
  {
    case AlListener::LT_Camera:   tp = rd->GetCamAbsTrans(); break;
    case AlListener::LT_Absolute: tp = &rd->ToGCS();         break;
  }
  ZTrans&  t = *tp;
  Float_t orient[6] = { (float) t(1,1), (float) t(2,1), (float) t(3,1),
                        (float) t(1,3), (float) t(2,3), (float) t(3,3) };
  alListener3f(AL_POSITION,    t(1,4), t(2,4), t(3,4));
  alListenerfv(AL_ORIENTATION, orient);
}

//void AlListener_GL_Rnr::PostDraw(RnrDriver* rd)
//{}
