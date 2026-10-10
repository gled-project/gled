// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "GTSurf_GL_Rnr.h"
#include <GTS/GTS.h>
#include <GL/glew.h>

using namespace gled;


#define PARENT ZNode_GL_Rnr

/**************************************************************************/

void GTSurf_GL_Rnr::Draw(RnrDriver* rd)
{
  GL_Capability_Switch _auto_norm(GL_NORMALIZE, true);
  PARENT::Draw(rd);
}

void GTSurf_GL_Rnr::Render(RnrDriver* rd)
{
  GTS::Mesh* m = mGTSurf->pMesh;
  if (!m) return;

  const gts::Geometrium& g = m->geo();

  glColor4fv(mGTSurf->mColor());
  glBegin(GL_TRIANGLES);
  for (gts::FaceId f : m->surf().faces())
  {
    const gts::Vec3 n = g.normal(f);
    glNormal3dv(&n.x);
    for (gts::VertexId v : g.vertices_of(f))
      glVertex3dv(&g.vertex(v).p.x);
  }
  glEnd();

  if (mGTSurf->bRnrPoints)
  {
    glPushAttrib(GL_ENABLE_BIT);
    glDisable(GL_LIGHTING);
    glColor4fv(mGTSurf->mPointColor());
    glBegin(GL_POINTS);
    for (gts::VertexId v : m->surf().vertices())
      glVertex3dv(&g.vertex(v).p.x);
    glEnd();
    glPopAttrib();
  }
}
