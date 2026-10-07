// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "Amphitheatre_GL_Rnr.h"
#include <Rnr/GL/GLRnrDriver.h>

#include <TMath.h>

using namespace gled;


/**************************************************************************/

void Amphitheatre_GL_Rnr::_init()
{
  mQuadric = gluNewQuadric();
  gluQuadricDrawStyle(mQuadric, GLU_SILHOUETTE);
  gluQuadricNormals(mQuadric, GLU_FLAT);
}

/**************************************************************************/
/*
void Amphitheatre_GL_Rnr::PreDraw(RnrDriver* rd)
{}
*/

void Amphitheatre_GL_Rnr::Draw(RnrDriver* rd)
{
  Amphitheatre& A = *mAmphitheatre;

  if(A.bRnrStage) {
    rd->GL()->Color(A.mStageCol);
    glPushMatrix();
    glRotated((0.5 + A.mStageRot) * 360 / A.mStageSides - 90, 0, 0, 1);
    gluDisk(mQuadric, 0, A.mStageSize, A.mStageSides, 1);
    glPopMatrix();
  }

  if(A.bRnrChairs) {
    rd->GL()->Color(A.mChairCol);
    for(Amphitheatre::lChair_i i = A.mChairs.begin();
	i != A.mChairs.end(); ++i)
      {
	glPushMatrix();
	ZPoint& p( i->fPos );
	Double_t phi = TMath::ATan2(p.y, p.x) * TMath::RadToDeg();
	glTranslated(p.x, p.y, p.z);
	glRotated(phi, 0, 0, 1);
	gluPartialDisk(mQuadric, 0, A.mChairSize, 7, 1, -45, 270);
	glPopMatrix();
      }
  }
}

/*
void Amphitheatre_GL_Rnr::PostDraw(RnrDriver* rd)
{}
*/
