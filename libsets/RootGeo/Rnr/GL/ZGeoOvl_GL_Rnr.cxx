// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ZGeoOvl_GL_Rnr.h"
#include <Rnr/GL/GLRnrDriver.h>
#include <GL/glew.h>

using namespace gled;

/**************************************************************************/

void ZGeoOvl_GL_Rnr::_init()
{}

/**************************************************************************/


void ZGeoOvl_GL_Rnr::Draw(RnrDriver* rd)
{
  if (mZGeoOvl->mRnrNode) {
    ZGeoNode_GL_Rnr::Draw(rd);
  }

  Float_t* p = mZGeoOvl->mPM_p;
  if (mZGeoOvl->mRnrMark && p != 0) {
    Int_t N = mZGeoOvl->mPM_N;
    rd->GL()->Color(mZGeoOvl->mPM_Col);
    glBegin(GL_POINTS);
    for(int i = 0; i<N; ++i) {
      glVertex3fv(p);
      p += 3;
    }
    glEnd();
  }
}
