// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "GTS.h"
#include "Stones/ZTrans.h"

#include "Gled/GMutex.h"
#include "Gled/GledNS.h"

using namespace gled;


GtsSurface* gled::MakeDefaultSurface()
{
  return gts_surface_new(gts_surface_class (), gts_face_class (),
			 gts_edge_class (),    gts_vertex_class ());
}

//==============================================================================

namespace
{
  int face_inverter(GtsFace* f, int* dum)
  {
    GtsEdge* egg = f->triangle.e1;
    f->triangle.e1 = f->triangle.e2;
    f->triangle.e2 = egg;
    return 0;
  }
}

void gled::InvertSurface(GtsSurface* s)
{
  gts_surface_foreach_face(s, (GtsFunc)face_inverter, 0);
}

//==============================================================================

namespace
{
  void vertex_transformer(GtsVertex* v, ZTrans* t)
  {
    t->MultiplyVec3IP(&v->p.x, 1);
  }

  void vertex_rotator(GtsVertex* v, ZTrans* t)
  {
    t->RotateVec3IP(&v->p.x);
  }

}

void gled::TransformSurfaceVertices(GtsSurface* s, ZTrans* t)
{
  gts_surface_foreach_vertex(s, (GtsFunc) vertex_transformer, t);
}

void gled::RotateSurfaceVertices(GtsSurface* s, ZTrans* t)
{
  gts_surface_foreach_vertex(s, (GtsFunc) vertex_rotator, t);
}

//==============================================================================

void gled::WriteSurfaceToFile(GtsSurface* s, const TString& file)
{
  FILE* fp = fopen(file, "w");
  if (!fp) {
    ISerr(GForm("WriteSurfaceToFile Can not open file '%s'.", file.Data()));
    return;
  }
  gts_surface_write(s, fp);
  fclose(fp);
}
