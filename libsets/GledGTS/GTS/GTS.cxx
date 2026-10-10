// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "GTS.h"
#include "Stones/ZTrans.h"

#include "Gled/GMutex.h"
#include "Gled/GledNS.h"

using namespace gled;


GTS::Mesh::Mesh() :
  m_geo(std::make_unique<gts::Geometrium>()), m_surf(&m_geo->new_surface())
{}

GTS::Mesh* GTS::Mesh::Copy() const
{
  auto g = std::make_unique<gts::Geometrium>();
  gts::Surface& s = g->import(*m_geo, *m_surf);
  return new Mesh(std::move(g), s);
}

//==============================================================================

void gled::InvertSurface(gts::Surface& s)
{
  for (gts::FaceId f : s.faces())
    s.geometrium().revert_face(f);
}

//==============================================================================

void gled::TransformSurfaceVertices(gts::Surface& s, const ZTrans& t)
{
  gts::Geometrium& g = s.geometrium();
  for (gts::VertexId v : s.vertices())
    t.MultiplyVec3IP(&g.position(v).x, 1);
}

void gled::RotateSurfaceVertices(gts::Surface& s, const ZTrans& t)
{
  gts::Geometrium& g = s.geometrium();
  for (gts::VertexId v : s.vertices())
    t.RotateVec3IP(&g.position(v).x);
}

//==============================================================================

void gled::WriteSurfaceToFile(const gts::Surface& s, const TString& file)
{
  if ( ! gts::write_gts(s, std::filesystem::path(file.Data())))
    ISerr(GForm("WriteSurfaceToFile Can not open file '%s'.", file.Data()));
}
