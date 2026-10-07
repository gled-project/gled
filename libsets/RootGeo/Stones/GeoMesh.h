// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef RootGeo_GeoMesh_H
#define RootGeo_GeoMesh_H

#include <Rtypes.h>

#include <vector>

class TBuffer3D;

namespace gled {

class GeoMesh
{
protected:
  static Bool_t eq(const Double_t* p1, const Double_t* p2);
  Int_t         check_points(const UInt_t* source, UInt_t* dest) const;

  void set_from_buffer(const TBuffer3D& buffer);
  void enforce_triangles();
  void calculate_normals();

public:
  std::vector<Double_t> fVertices; // x, y, z per vertex
  std::vector<Double_t> fNormals;  // x, y, z per triangle
  std::vector<UInt_t>   fPolyDesc; // per polygon: n, then n vertex indices
  std::vector<UChar_t>  fEdgeFlags; // per triangle vertex: its edge to the next vertex is on the polygon boundary
  Int_t                 fNPolys;

  GeoMesh(const TBuffer3D& buffer);
};

} // endnamespace gled

#endif
