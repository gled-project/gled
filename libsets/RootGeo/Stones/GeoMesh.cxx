// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

//__________________________________________________________________________
// GeoMesh
//
// Triangle mesh of a TGeoShape, built from the raw sections of its
// TBuffer3D: the polygons, given there as lists of segments, are turned
// into vertex loops, split into triangles with the GLU tessellator and
// given one normal per triangle. The edge flags mark the triangle edges
// that lie on the boundary of the original polygon, so that a wireframe
// drawing shows the polygons and not their triangulation.
//
// Adapted from REveGeoPolyShape and REveGluTess of ROOT's Eve7.

#include "GeoMesh.h"

#include <TBuffer3D.h>
#include <TMath.h>

#include <GL/glu.h>

#include <deque>

using namespace gled;

//==========================================================================
// Triangle collector, driving the GLU tessellator
//==========================================================================

namespace
{
  class TriangleCollector
  {
  public:
    GLUtesselator         *fTess;
    std::vector<Double_t> &fVertices;
    std::vector<UInt_t>    fPolyDesc;
    std::vector<UChar_t>   fEdgeFlags;
    UChar_t                fEdgeFlag;
    std::deque<UInt_t>     fNewIndices; // stable storage for combined vertices
    Int_t                  fNTriangles;
    Int_t                  fNVertices;
    Int_t                  fV0, fV1;
    UChar_t                fE0, fE1;
    GLenum                 fType;

    TriangleCollector(std::vector<Double_t>& verts);
    ~TriangleCollector() { gluDeleteTess(fTess); }

    void add_triangle(UInt_t v0, UInt_t v1, UInt_t v2, UChar_t e0, UChar_t e1, UChar_t e2);
    void process_vertex(UInt_t vi);
    void process(const std::vector<UInt_t>& polys, Int_t n_polys);

    static void tess_begin(GLenum type, TriangleCollector* tc);
    static void tess_vertex(UInt_t* vi, TriangleCollector* tc);
    static void tess_combine(GLdouble coords[3], void* vertex_data[4],
                             GLfloat weight[4], void** out_data,
                             TriangleCollector* tc);
    static void tess_edge_flag(GLboolean flag, TriangleCollector* tc);
    static void tess_end(TriangleCollector* tc);
  };

  TriangleCollector::TriangleCollector(std::vector<Double_t>& verts) :
    fVertices(verts), fEdgeFlag(1), fNTriangles(0), fNVertices(0),
    fV0(-1), fV1(-1), fE0(1), fE1(1), fType(GL_NONE)
  {
    fTess = gluNewTess();
    if (!fTess) throw std::bad_alloc();

    gluTessCallback(fTess, GLU_TESS_BEGIN_DATA,   (_GLUfuncptr) tess_begin);
    gluTessCallback(fTess, GLU_TESS_VERTEX_DATA,  (_GLUfuncptr) tess_vertex);
    gluTessCallback(fTess, GLU_TESS_COMBINE_DATA, (_GLUfuncptr) tess_combine);
    // With an edge-flag callback GLU emits only separate triangles.
    gluTessCallback(fTess, GLU_TESS_EDGE_FLAG_DATA, (_GLUfuncptr) tess_edge_flag);
    gluTessCallback(fTess, GLU_TESS_END_DATA,     (_GLUfuncptr) tess_end);
  }

  void TriangleCollector::add_triangle(UInt_t v0, UInt_t v1, UInt_t v2,
                                       UChar_t e0, UChar_t e1, UChar_t e2)
  {
    fPolyDesc.push_back(3);
    fPolyDesc.push_back(v0);
    fPolyDesc.push_back(v1);
    fPolyDesc.push_back(v2);
    fEdgeFlags.push_back(e0);
    fEdgeFlags.push_back(e1);
    fEdgeFlags.push_back(e2);
    ++fNTriangles;
  }

  void TriangleCollector::process_vertex(UInt_t vi)
  {
    ++fNVertices;

    // The edge-flag callback restricts GLU to GL_TRIANGLES.
    if (fV0 == -1) { fV0 = vi; fE0 = fEdgeFlag; return; }
    if (fV1 == -1) { fV1 = vi; fE1 = fEdgeFlag; return; }

    if (fType == GL_TRIANGLES)
      add_triangle(fV0, fV1, vi, fE0, fE1, fEdgeFlag);
    fV0 = fV1 = -1;
  }

  void TriangleCollector::process(const std::vector<UInt_t>& polys, Int_t n_polys)
  {
    if (fVertices.empty() || polys.empty())
      return;

    const UInt_t *pols = &polys[0];

    for (Int_t i = 0, j = 0; i < n_polys; ++i)
    {
      // Combined vertices of the previous polygon may have reallocated
      // the vertex array. GLU copies the coordinates in gluTessVertex().
      Double_t *pnts = &fVertices[0];
      Int_t n_points = pols[j++];

      gluTessBeginPolygon(fTess, this);
      gluTessBeginContour(fTess);
      for (Int_t k = 0; k < n_points; ++k, ++j)
      {
        gluTessVertex(fTess, pnts + pols[j] * 3, (GLvoid*) &pols[j]);
      }
      gluTessEndContour(fTess);
      gluTessEndPolygon(fTess);
    }
  }

  void TriangleCollector::tess_begin(GLenum type, TriangleCollector* tc)
  {
    tc->fNVertices = 0;
    tc->fV0 = tc->fV1 = -1;
    tc->fType = type;
  }

  void TriangleCollector::tess_vertex(UInt_t* vi, TriangleCollector* tc)
  {
    tc->process_vertex(*vi);
  }

  void TriangleCollector::tess_combine(GLdouble coords[3], void* /*vertex_data*/[4],
                                       GLfloat /*weight*/[4], void** out_data,
                                       TriangleCollector* tc)
  {
    // Self-intersecting or touching contours: add the new vertex.
    tc->fNewIndices.push_back(tc->fVertices.size() / 3);
    tc->fVertices.insert(tc->fVertices.end(), coords, coords + 3);
    *out_data = &tc->fNewIndices.back();
  }

  void TriangleCollector::tess_edge_flag(GLboolean flag, TriangleCollector* tc)
  {
    tc->fEdgeFlag = flag;
  }

  void TriangleCollector::tess_end(TriangleCollector* tc)
  {
    tc->fType = GL_NONE;
  }
}

//==========================================================================
// GeoMesh
//==========================================================================

GeoMesh::GeoMesh(const TBuffer3D& buffer) :
  fNPolys(0)
{
  set_from_buffer(buffer);
  enforce_triangles();
  calculate_normals();
}

/**************************************************************************/

void GeoMesh::set_from_buffer(const TBuffer3D& buffer)
{
  // Converts the polygons of the buffer, lists of segments, into lists of
  // vertices.

  fNPolys = (Int_t) buffer.NbPols();
  if (fNPolys == 0) return;

  fVertices.assign(buffer.fPnts, buffer.fPnts + 3 * buffer.NbPnts());

  const Int_t *segs = buffer.fSegs;
  const Int_t *pols = buffer.fPols;

  Int_t desc_size = 0;
  for (Int_t i = 0, j = 1; i < fNPolys; ++i, ++j)
  {
    desc_size += pols[j] + 1;
    j += pols[j] + 1;
  }
  fPolyDesc.resize(desc_size);

  for (Int_t n_pol = 0, cur = 0, j = 1; n_pol < fNPolys; ++n_pol)
  {
    Int_t seg_ind = pols[j] + j;
    Int_t seg_cnt = pols[j];
    Int_t s1 = pols[seg_ind--];
    Int_t s2 = pols[seg_ind--];
    Int_t seg_ends[] = { segs[s1 * 3 + 1], segs[s1 * 3 + 2],
                         segs[s2 * 3 + 1], segs[s2 * 3 + 2] };
    Int_t pnts[3];

    if (seg_ends[0] == seg_ends[2]) {
      pnts[0] = seg_ends[1]; pnts[1] = seg_ends[0]; pnts[2] = seg_ends[3];
    } else if (seg_ends[0] == seg_ends[3]) {
      pnts[0] = seg_ends[1]; pnts[1] = seg_ends[0]; pnts[2] = seg_ends[2];
    } else if (seg_ends[1] == seg_ends[2]) {
      pnts[0] = seg_ends[0]; pnts[1] = seg_ends[1]; pnts[2] = seg_ends[3];
    } else {
      pnts[0] = seg_ends[0]; pnts[1] = seg_ends[1]; pnts[2] = seg_ends[2];
    }

    fPolyDesc[cur] = 3;
    Int_t size_ind = cur++;
    fPolyDesc[cur++] = pnts[0];
    fPolyDesc[cur++] = pnts[1];
    fPolyDesc[cur++] = pnts[2];
    Int_t last = pnts[2];

    for (Int_t end = j + 1; seg_ind != end; --seg_ind)
    {
      seg_ends[0] = segs[pols[seg_ind] * 3 + 1];
      seg_ends[1] = segs[pols[seg_ind] * 3 + 2];
      last = (seg_ends[0] == last) ? seg_ends[1] : seg_ends[0];
      fPolyDesc[cur++] = last;
      ++fPolyDesc[size_ind];
    }
    j += seg_cnt + 2;
  }
}

void GeoMesh::enforce_triangles()
{
  // Replaces the polygons with triangles from the GLU tessellator.

  TriangleCollector tc(fVertices);
  tc.process(fPolyDesc, fNPolys);
  fPolyDesc.swap(tc.fPolyDesc);
  fEdgeFlags.swap(tc.fEdgeFlags);
  fNPolys = tc.fNTriangles;
}

void GeoMesh::calculate_normals()
{
  // One normal per polygon, from its first three distinct vertices.

  fNormals.assign(3 * fNPolys, 0);
  if (fNPolys == 0) return;

  const Double_t *pnts = &fVertices[0];
  for (Int_t i = 0, j = 0; i < fNPolys; ++i)
  {
    Int_t  pol_end = fPolyDesc[j] + j + 1;
    UInt_t norm[]  = { fPolyDesc[j + 1], fPolyDesc[j + 2], fPolyDesc[j + 3] };
    j += 4;
    Int_t ngood = check_points(norm, norm);
    if (ngood == 3)
    {
      TMath::Normal2Plane(pnts + norm[0] * 3, pnts + norm[1] * 3,
                          pnts + norm[2] * 3, &fNormals[i * 3]);
      j = pol_end;
      continue;
    }
    while (j < pol_end)
    {
      norm[ngood++] = fPolyDesc[j++];
      if (ngood == 3)
      {
        ngood = check_points(norm, norm);
        if (ngood == 3)
        {
          TMath::Normal2Plane(pnts + norm[0] * 3, pnts + norm[1] * 3,
                              pnts + norm[2] * 3, &fNormals[i * 3]);
          j = pol_end;
          break;
        }
      }
    }
  }
}

/**************************************************************************/

Int_t GeoMesh::check_points(const UInt_t* source, UInt_t* dest) const
{
  // Copies the distinct ones among the three vertices to dest and returns
  // their number.

  const Double_t *p1 = &fVertices[source[0] * 3];
  const Double_t *p2 = &fVertices[source[1] * 3];
  const Double_t *p3 = &fVertices[source[2] * 3];
  Int_t ret = 1;

  if (eq(p1, p2))
  {
    dest[0] = source[0];
    if (!eq(p1, p3))
    {
      dest[1] = source[2];
      ret = 2;
    }
  }
  else if (eq(p1, p3))
  {
    dest[0] = source[0];
    dest[1] = source[1];
    ret = 2;
  }
  else
  {
    dest[0] = source[0];
    dest[1] = source[1];
    ret = 2;
    if (!eq(p2, p3))
    {
      dest[2] = source[2];
      ret = 3;
    }
  }
  return ret;
}

Bool_t GeoMesh::eq(const Double_t* p1, const Double_t* p2)
{
  return TMath::Abs(p1[0] - p2[0]) < 1e-10 &&
         TMath::Abs(p1[1] - p2[1]) < 1e-10 &&
         TMath::Abs(p1[2] - p2[2]) < 1e-10;
}
