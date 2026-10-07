// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "GTSurf_GL_Rnr.h"
#include <GTS/GTS.h>
#include <GL/glew.h>

using namespace gled;


#define PARENT ZNode_GL_Rnr

/**************************************************************************/

namespace
{
  void face_drawer(GtsFace* f, void*)
  {
    /*
      GtsPoint& p1 = t->e1->segment.v1->p;
      GtsPoint& p2 = t->e2->segment.v2->p;
      glVertex3d(p1.x, p1.y, p1.z);
      glVertex3d(p2.x, p2.y, p2.z);
    */
    gdouble     n[3];
    GtsVertex* vp[3];
    gts_triangle_normal(&f->triangle, &n[0], &n[1], &n[2]);
    glNormal3dv(n);
    gts_triangle_vertices(&f->triangle, &vp[0], &vp[1], &vp[2]);
    glVertex3dv(&vp[0]->p.x);
    glVertex3dv(&vp[1]->p.x);
    glVertex3dv(&vp[2]->p.x);
  }

  void vertex_drawer(GtsVertex* v, void*)
  {
    glVertex3dv(&v->p.x);
  }
}

void GTSurf_GL_Rnr::Draw(RnrDriver* rd)
{
  GL_Capability_Switch _auto_norm(GL_NORMALIZE, true);
  PARENT::Draw(rd);
}

void GTSurf_GL_Rnr::Render(RnrDriver* rd)
{
  if (!mGTSurf->pSurf) return;

  glColor4fv(mGTSurf->mColor());
  glBegin(GL_TRIANGLES);
  gts_surface_foreach_face(mGTSurf->pSurf, (GtsFunc)face_drawer, 0);
  glEnd();

  if (mGTSurf->bRnrPoints)
  {
    glPushAttrib(GL_ENABLE_BIT);
    glDisable(GL_LIGHTING);
    glColor4fv(mGTSurf->mPointColor());
    glBegin(GL_POINTS);
    gts_surface_foreach_vertex(mGTSurf->pSurf, (GtsFunc)vertex_drawer, 0);
    glEnd();
    glPopAttrib();
  }
}
