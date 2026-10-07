// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ZGeoNode_GL_Rnr.h"
#include <Rnr/GL/GLRnrDriver.h>

using namespace gled;

#include <Stones/GeoMesh.h>

#include <GL/glew.h>

/**************************************************************************/

void ZGeoNode_GL_Rnr::_init()
{}

/**************************************************************************/

void ZGeoNode_GL_Rnr::Draw(RnrDriver* rd)
{
  ZGeoNode& N(*mZGeoNode);
  GeoUserData *ud = dynamic_cast<GeoUserData*>(N.GetVolumeField());
  if ( ud == 0 ) return;

  const GeoMesh* mesh = ud->fMesh;
  if (mesh == 0 || mesh->fNPolys == 0) return;

  Float_t alpha = rd->GL()->Color(N.mColor);
  if (alpha < 1) {
    glPushAttrib(GL_CURRENT_BIT | GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    glEnable(GL_BLEND);
    glDepthMask(GL_FALSE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  } else {
    glPushAttrib(GL_CURRENT_BIT);
  }

  const Double_t *pnts  = &mesh->fVertices[0];
  const Double_t *norms = &mesh->fNormals[0];
  const UInt_t   *pols  = &mesh->fPolyDesc[0];
  const UChar_t  *edges = &mesh->fEdgeFlags[0];
  // Edge flags keep the triangulation out of GL_LINE polygon mode.
  glBegin(GL_TRIANGLES);
  for (Int_t i = 0, j = 0; i < mesh->fNPolys; ++i, j += 4)
  {
    glNormal3dv(norms + 3 * i);
    for (Int_t k = 0; k < 3; ++k)
    {
      glEdgeFlag(edges[3 * i + k]);
      glVertex3dv(pnts + 3 * pols[j + 1 + k]);
    }
  }
  glEdgeFlag(GL_TRUE);
  glEnd();

  glPopAttrib();
}
