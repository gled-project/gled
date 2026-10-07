// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "Crawler_GL_Rnr.h"
#include <GL/glew.h>

using namespace gled;


#define PARENT Extendio_GL_Rnr

//==============================================================================

Crawler_GL_Rnr::Crawler_GL_Rnr(Crawler* idol) :
  Extendio_GL_Rnr(idol),
  mCrawler(idol)
{
  // Init laser directions to something silly.
  mLastLaserUpDn = mLastLaserLtRt = 1e20f;
}

Crawler_GL_Rnr::~Crawler_GL_Rnr()
{}

//==============================================================================

// void Crawler_GL_Rnr::PreDraw(RnrDriver* rd) {}

void Crawler_GL_Rnr::Draw(RnrDriver* rd)
{
  PARENT::Draw(rd);

  Crawler &C = * mCrawler;

  if (mLastLaserUpDn != C.mLaserUpDn || mLastLaserLtRt != C.mLaserLtRt)
  {
    mLaserEnd = C.mLaserBeg;
    const Float_t p = mLastLaserLtRt = C.mLaserLtRt;
    const Float_t t = mLastLaserUpDn = C.mLaserUpDn;
    const Float_t ct = cosf(t);
    mLaserEnd.Add(ct*cosf(p), ct*sinf(p), sinf(t));
  }

  GL_Float_Holder(GL_LINE_WIDTH, 3.0f, glLineWidth);
  glColor3f(0.0f, 0.0f, 0.0f);
  glBegin(GL_LINES);
  glVertex3fv(mCrawler->RefLaserBeg());
  glVertex3fv(mLaserEnd);
  glEnd();
}

// void Crawler_GL_Rnr::PostDraw(RnrDriver* rd) {}

// void Crawler_GL_Rnr::Render(RnrDriver* rd) {}
