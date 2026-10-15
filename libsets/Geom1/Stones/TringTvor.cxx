// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

//==============================================================================
// TringTvor
//==============================================================================

//______________________________________________________________________
//
// Encapsulates low-level arrays for triangle meshes.
// Vertex and Triangle arrays are mandatory (called primary by methods).
// Optional:
//   per-vertex   normals and colors |
//   per-triangle normals and colors +-> secondary arrays
//   texture coords (per-vertex)     |
//   triangle strip data
//   patches, contiguous runs of triangles with bounds for culling
//
// Serialization should mostly work. Secondary data is streamed as
// well.  As triangle-attributes (normals, colors) are usually
// calculated from vertex-attributes, some provision might be necessary to
// streamline this. Anyway ... this should really be managed by the lens
// using the tring-tvor as it also has the AdEnlightenment() virtual where
// post-serialization / pre-usage chores can be performed.
//
// Strip and patch data are not saved, nor is there any indication they were used.
//
// Jul 2011
// This should really be templated for float / double.
// Texture arrays should be moved out and make independent.
// Maybe also the normal arrays -- or at least they should be templated with a
// different type so one can use char or short.

#include "TringTvor.h"

#include <TMath.h>
#include <TRandom.h>

#include <atomic>
#include <cmath>

#include <ACTC/tc.h>

using namespace gled;

#define INV3                    0.33333333333333333333f

namespace {
  std::atomic<UInt_t> s_patch_stamp(0);
}

void TringTvor::_init()
{
  mBBoxOK = false;
  mMaxVertexDistance = mBBoxHalfDiagonal = 0;
  mMinEdgeLen = mMaxEdgeLen = 0;

  mNStripEls   = 0;  mStripEls    = 0;  mStripTrings = 0;
  mNStrips     = 0;  mStripBegs   = 0;  mStripLens   = 0;

  mPatchStamp  = 0;
}

TringTvor::TringTvor() :
  mNVerts  (0),
  mNTrings (0),
  mNormalMode  (M_None),
  mColorMode   (M_None),
  mTextureMode (M_None)
{
  _init();
}

TringTvor::TringTvor(Int_t nv, Int_t nt) :
  mNVerts  (nv),
  mNTrings (nt),
  mNormalMode  (M_None),
  mColorMode   (M_None),
  mTextureMode (M_None)
{
  _init();
  MakePrimaryArrays();
}

TringTvor::TringTvor(Int_t nv, Int_t nt, Mode_e normal, Mode_e color, Mode_e texture) :
  mNVerts      (nv),
  mNTrings     (nt),
  mNormalMode  (normal),
  mColorMode   (color),
  mTextureMode (texture)
{
  _init();
  MakePrimaryArrays();
  MakeSecondaryArrays();
}

TringTvor::TringTvor(Int_t nv, Int_t nt, Bool_t smoothp, Bool_t colp, Bool_t texp) :
  mNVerts      (nv),
  mNTrings     (nt),
  mNormalMode  (smoothp ? M_PerVertex : M_PerTriangle),
  mColorMode   (colp ? (smoothp ? M_PerVertex : M_PerTriangle) : M_None),
  mTextureMode (texp ? M_PerVertex : M_None)
{
  _init();
  MakePrimaryArrays();
  MakeSecondaryArrays();
}

TringTvor::~TringTvor()
{
  DeleteTriangleStrips();
  DeleteSecondaryArrays();
  DeletePrimaryArrays();
}

void TringTvor::Reset(Int_t nv, Int_t nt)
{
  WipePatches();
  DeleteTriangleStrips();
  DeleteSecondaryArrays();
  DeletePrimaryArrays();
  mNVerts      = nv;
  mNTrings     = nt;
  mNormalMode  = M_None;
  mColorMode   = M_None;
  mTextureMode = M_None;
  MakePrimaryArrays();
  mBBoxOK = false;
}

void TringTvor::Reset(Int_t nv, Int_t nt, Mode_e normal, Mode_e color, Mode_e texture)
{
  WipePatches();
  DeleteTriangleStrips();
  DeleteSecondaryArrays();
  DeletePrimaryArrays();
  mNVerts      = nv;
  mNTrings     = nt;
  mNormalMode  = normal;
  mColorMode   = color;
  mTextureMode = texture;
  MakePrimaryArrays();  
  MakeSecondaryArrays();
  mBBoxOK = false;
}

/**************************************************************************/

void TringTvor::MakePrimaryArrays()
{
  mVerts .resize(3*mNVerts);
  mTrings.resize(3*mNTrings);
}

void TringTvor::DeletePrimaryArrays()
{
  WipeVerts();  mNVerts  = 0;
  WipeTrings(); mNTrings = 0;
}

void TringTvor::MakeSecondaryArrays()
{
  switch (mNormalMode)
  {
    case M_PerVertex:   AssertNorms();      break;
    case M_PerTriangle: AssertTringNorms(); break;
    default: break;
  }
  switch (mColorMode)
  {
    case M_PerVertex:   AssertCols();      break;
    case M_PerTriangle: AssertTringCols(); break;
    default: break;
  }
  switch (mTextureMode)
  {
    case M_PerVertex:   AssertTexs();      break;
    case M_PerTriangle: AssertTringTexs(); break;
    default: break;
  }
}

void TringTvor::DeleteSecondaryArrays()
{
  WipeNorms();
  WipeCols();
  WipeTexs();

  WipeTringNorms();
  WipeTringCols();
  WipeTringTexs();
}

/**************************************************************************/

Int_t TringTvor::AddVertices(Int_t nv)
{
  // Returns old number of vertices.

  Int_t nold = mNVerts;
  WipePatches();
  mNVerts += nv;
  mVerts.resize(3*mNVerts);
  if (HasNorms()) MakeNorms();
  if (HasCols())  MakeCols();
  if (HasTexs())  MakeTexs();
  mBBoxOK = false;
  return nold;
}

Int_t TringTvor::AddTriangles(Int_t nt)
{
  // Returns old number of triangles.

  Int_t nold = mNTrings;
  WipePatches();
  mNTrings += nt;
  mTrings.resize(3*mNTrings);
  if (HasTringNorms()) MakeTringNorms();
  if (HasTringCols())  MakeTringCols();
  if (HasTringTexs())  MakeTringTexs();
  mBBoxOK = false;
  return nold;
}

/**************************************************************************/

void TringTvor::CalculateBoundingBox()
{
  if (mNVerts == 0 || mNTrings == 0)
  {
    for (Int_t i = 0; i < 6; ++i)
    {
      mMinMaxBox[i] = mCtrExtBox[i] = 0;
    }
    mMaxVertexDistance = mMinEdgeLen = mMaxEdgeLen = 0;
    return;
  }

  Float_t *V = Verts(), *m = mMinMaxBox, *M = m + 3;
  m[0] = M[0] = V[0];
  m[1] = M[1] = V[1];
  m[2] = M[2] = V[2];
  mMaxVertexDistance = V[0]*V[0] + V[1]*V[1] + V[2]*V[2];
  V += 3;
  for (Int_t v = 1; v < mNVerts; ++v, V+=3)
  {
    if (V[0] < m[0]) m[0] = V[0]; else if (V[0] > M[0]) M[0] = V[0];
    if (V[1] < m[1]) m[1] = V[1]; else if (V[1] > M[1]) M[1] = V[1];
    if (V[2] < m[2]) m[2] = V[2]; else if (V[2] > M[2]) M[2] = V[2];

    Float_t vd = V[0]*V[0] + V[1]*V[1] + V[2]*V[2];
    if (vd > mMaxVertexDistance)
      mMaxVertexDistance = vd;
  }
  mMaxVertexDistance = sqrtf(mMaxVertexDistance);

  Float_t *C = mCtrExtBox, *E = C + 3;
  C[0] = 0.5f*(M[0]+m[0]); C[1] = 0.5f*(M[1]+m[1]); C[2] = 0.5f*(M[2]+m[2]);
  E[0] = 0.5f*(M[0]-m[0]); E[1] = 0.5f*(M[1]-m[1]); E[2] = 0.5f*(M[2]-m[2]);

  mBBoxHalfDiagonal = sqrtf(E[0]*E[0] + E[1]*E[1] + E[2]*E[2]);

  // Find min/max edge length.
  // Not optimal, we visit most of the edges twice.
  SqrMinMaxEdgeLen(0, mMinEdgeLen, mMaxEdgeLen);
  for (Int_t t = 1; t < mNTrings; ++t)
  {
    Float_t min, max;
    SqrMinMaxEdgeLen(t, min, max);
    if (min < mMinEdgeLen) mMinEdgeLen = min;
    if (max > mMaxEdgeLen) mMaxEdgeLen = max;
  }
  mMinEdgeLen = sqrtf(mMinEdgeLen);
  mMaxEdgeLen = sqrtf(mMaxEdgeLen);

  mBBoxOK = true;
}

Float_t TringTvor::BoundingBoxXYArea() const
{
  const Float_t *E = mCtrExtBox + 3;
  return 4.0f * E[0]*E[1];
}

Float_t TringTvor::BoundingBoxVolume() const
{
  const Float_t *E = mCtrExtBox + 3;
  return 8.0f * E[0] * E[1] * E[2];
}

/**************************************************************************/

Float_t TringTvor::CalculateTriangleNormal(Int_t ti, Float_t normal[3])
{
  // Calculates triangle normal from vertex-data and stores it into 'normal'.
  // Returns original norm of the vector.
  //
  // This is to be used when triangle normals are not stored or during
  // construction of the tvor.

  const Int_t*    T = Triangle(ti);
  const Float_t* v0 = Vertex(T[0]);
  const Float_t* v1 = Vertex(T[1]);
  const Float_t* v2 = Vertex(T[2]);
  Float_t e1[3], e2[3];
  e1[0] = v1[0]-v0[0]; e1[1] = v1[1]-v0[1]; e1[2] = v1[2]-v0[2];
  e2[0] = v2[0]-v0[0]; e2[1] = v2[1]-v0[1]; e2[2] = v2[2]-v0[2];

  return TMath::NormCross(e1, e2, normal);
}

Float_t TringTvor::CalculateTriangleNormalAndCog(Int_t ti, Float_t normal[3], Float_t cog[3])
{
  // Calculates triangle normal from vertex-data and stores it into
  // 'normal'. Returns original norm of the vector. Center-of-gravity
  // of the triangle is returned in 'cog'.
  //
  // This is to be used when triangle normals are not stored or during
  // construction of the tvor.

  const Int_t*    T = Triangle(ti);
  const Float_t* v0 = Vertex(T[0]);
  const Float_t* v1 = Vertex(T[1]);
  const Float_t* v2 = Vertex(T[2]);
  Float_t e1[3], e2[3];
  e1[0] = v1[0]-v0[0]; e1[1] = v1[1]-v0[1]; e1[2] = v1[2]-v0[2];
  e2[0] = v2[0]-v0[0]; e2[1] = v2[1]-v0[1]; e2[2] = v2[2]-v0[2];

  cog[0] = (v0[0] + v1[0] + v2[0]) * INV3;
  cog[1] = (v0[1] + v1[1] + v2[1]) * INV3;
  cog[2] = (v0[2] + v1[2] + v2[2]) * INV3;

  return TMath::NormCross(e1, e2, normal);
}

void TringTvor::GenerateTriangleNormals()
{
  AssertTringNorms();

  for(Int_t t=0; t<mNTrings; ++t) 
  {
    CalculateTriangleNormal(t, TriangleNormal(t));
  }
}

void TringTvor::GenerateTriangleNormalsAndColors
  (void (*foo)(Float_t*, UChar_t*, void*), void* ud)
{
  AssertTringNorms();
  AssertTringCols();

  Float_t  cog[3];

  for(Int_t t=0; t<mNTrings; ++t)
  {
    CalculateTriangleNormalAndCog(t, TriangleNormal(t), cog);
    foo(cog, TriangleColor(t), ud);
  }
}

//------------------------------------------------------------------------------

void TringTvor::GenerateTriangleColorsFromVertexColors()
{
  AssertTringCols();
  GenerateTriangleColorsFromVertexColors(Cols(), TringCols());
}

void TringTvor::GenerateTriangleColorsFromVertexColors(UChar_t* VCA, UChar_t* TCA)
{
  Int_t   *T = Trings();
  UChar_t *C = TCA;
  for (Int_t t=0; t<mNTrings; ++t, T+=3, C+=4)
  {
    UChar_t *c0 = &VCA[4*T[0]];
    UChar_t *c1 = &VCA[4*T[1]];
    UChar_t *c2 = &VCA[4*T[2]];
    C[0] = (UChar_t) (((UInt_t) c0[0] + c1[0] + c2[0]) * INV3);
    C[1] = (UChar_t) (((UInt_t) c0[1] + c1[1] + c2[1]) * INV3);
    C[2] = (UChar_t) (((UInt_t) c0[2] + c1[2] + c2[2]) * INV3);
    C[3] = (UChar_t) (((UInt_t) c0[3] + c1[3] + c2[3]) * INV3);
  }
}

void TringTvor::GenerateTriangleColorsFromVertexColors(std::set<Int_t>& triangles)
{
  AssertTringCols();
  GenerateTriangleColorsFromVertexColors(triangles, Cols(), TringCols());
}

void TringTvor::GenerateTriangleColorsFromVertexColors(std::set<Int_t>& triangles, UChar_t* VCA, UChar_t* TCA)
{
  for (std::set<Int_t>::iterator t = triangles.begin(); t != triangles.end(); ++t)
  {
    Int_t   *T  = Triangle(*t);
    UChar_t *C  = &TCA[4*(*t)];
    UChar_t *c0 = &VCA[4*T[0]];
    UChar_t *c1 = &VCA[4*T[1]];
    UChar_t *c2 = &VCA[4*T[2]];
    C[0] = (UChar_t) (((UInt_t) c0[0] + c1[0] + c2[0]) * INV3);
    C[1] = (UChar_t) (((UInt_t) c0[1] + c1[1] + c2[1]) * INV3);
    C[2] = (UChar_t) (((UInt_t) c0[2] + c1[2] + c2[2]) * INV3);
    C[3] = (UChar_t) (((UInt_t) c0[3] + c1[3] + c2[3]) * INV3);
  }
}

//------------------------------------------------------------------------------

void TringTvor::GenerateTriangleColorsFromTriangleStrips()
{
  // Assign random colors to different triangle strips.

  if (!HasTrianlgeStrips())
    return;
  AssertTringCols();

  TRandom rnd(0);

  for (Int_t i = 0; i < mNStrips; ++i)
  {
    UChar_t r = (UChar_t) rnd.Integer(256);
    UChar_t g = (UChar_t) rnd.Integer(256);
    UChar_t b = (UChar_t) rnd.Integer(256);

    Int_t* tring_idxp = mStripTrings + (mStripBegs[i] - mStripEls) + 2;
    Int_t  n          = mStripLens[i] - 2;
    while (n-- > 0)
    {
      UChar_t* tc = TriangleColor(*tring_idxp);
      tc[0] = r; tc[1] = g; tc[2] = b;
      ++tring_idxp;
    }
  }
}

//------------------------------------------------------------------------------

void TringTvor::GenerateVertexNormals()
{
  // Generate per-vertex normals by averaging over normals of all triangles
  // sharing the vertex.
  // Requires building of intermediate structure.
  //
  // ??? Need an argument: weight normal contribution by triangle area
  // ??? or ... perhaps better ... by vertex angle.
  //     This is somewhat done in vertex-connection stuff in Var1::TriMesh.

  // Could reuse tring-normals if they do exist.

  std::vector<Int_t> *trings_per_vert = new std::vector<Int_t> [mNVerts];
  FindTrianglesPerVertex(trings_per_vert);

  AssertNorms();

  Float_t *v0, *v1, *v2;
  Int_t   *t;
  Float_t e1[3], e2[3], n[3];
  Float_t* N = &mNorms[0];
  for (Int_t i=0; i<mNVerts; ++i, N+=3) {
    std::vector<Int_t>& v = trings_per_vert[i];
    Int_t       size = (Int_t) v.size();
    //printf("V=%3d : Nt = %2d : ", i, size);
    N[0] = N[1] = N[2] = 0;
    for (Int_t j=0; j<size; ++j) {
      //printf("%3d ", v[j]);
      t  = Triangle(v[j]);
      v0 = Vertex(t[0]); v1 = Vertex(t[1]); v2 = Vertex(t[2]);
      e1[0] = v1[0] - v0[0]; e1[1] = v1[1] - v0[1]; e1[2] = v1[2] - v0[2];
      e2[0] = v2[0] - v0[0]; e2[1] = v2[1] - v0[1]; e2[2] = v2[2] - v0[2];
      TMath::NormCross(e1, e2, n);
      N[0] += n[0]; N[1] += n[1]; N[2] += n[2];
    }
    TMath::Normalize(N);
    //printf(" : %f %f %f\n", N[0], N[1], N[2]);
  }

  delete [] trings_per_vert;
}

/**************************************************************************/
// Intermediate structures
/**************************************************************************/

void TringTvor::FindTrianglesPerVertex(std::vector<Int_t>* trings_per_vert)
{
  // Populate array of vectors 'trings_per_vert' from triangle data.
  // The output array must be properly allocated in advance.

  Int_t* T = Trings();
  for (Int_t t=0; t<mNTrings; ++t, T+=3)
  {
    trings_per_vert[T[0]].push_back(t);
    trings_per_vert[T[1]].push_back(t);
    trings_per_vert[T[2]].push_back(t);
  }
}

Int_t TringTvor::FindNeighboursPerVertex(std::vector<Int_t>* neighbours)
{
  // Populate array of vectors 'neighbours' from triangle data.
  // The output array must be properly allocated in advance.
  // Returns total number of connections (which is 2*N_edge for closed
  // surfaces).

  const Int_t vP[3][2] = { { 0, 1 }, { 1, 2 }, { 2, 0 } };

  Int_t  nc = 0;
  Int_t* T  = Trings();
  for (Int_t t=0; t<mNTrings; ++t, T+=3)
  {
    for (Int_t eti=0; eti<3; ++eti)
    {
      const  Int_t   v0 = T[vP[eti][0]],   v1 = T[vP[eti][1]];
      std::vector<Int_t> &n0 = neighbours[v0], &n1 = neighbours[v1];

      if (find(n0.begin(), n0.end(), v1) == n0.end())
      { n0.push_back(v1); ++nc; }
      if (find(n1.begin(), n1.end(), v0) == n1.end())
      { n1.push_back(v0); ++nc; }
    }
  }
  return nc;
}


/**************************************************************************/
// Triangle strips
/**************************************************************************/

namespace {
  struct xx_tring {
    Int_t v1,v2,v3;
    xx_tring(Int_t _v1, Int_t _v2, Int_t _v3) : v1(_v1), v2(_v2), v3(_v3) {}

    bool operator==(const xx_tring& x) const
    // { return v1==x.v1 && v2==x.v2 && v3==x.v3;}
    { return
	(v1==x.v1 || v1==x.v2 || v1==x.v3) &&
	(v2==x.v1 || v2==x.v2 || v2==x.v3) &&
	(v3==x.v1 || v3==x.v2 || v3==x.v3);}
  };
}
namespace std {
  template<>
  struct hash<xx_tring> {
    size_t operator()(const xx_tring& xx) const
    { size_t i = xx.v1 * xx.v2 * xx.v3; return i; }
  };
}

void TringTvor::GenerateTriangleStrips(Int_t max_verts)
{
  static const Exc_t _eh("TringTvor::GenerateTriangleStrips ");

  std::unordered_map<xx_tring, Int_t> tring_map;
  for (Int_t t = 0; t < mNTrings; ++t)
  {
    Int_t* T = Triangle(t);
    tring_map[ xx_tring(T[0], T[1], T[2]) ] = t;
  }

  int cnt = 0;
  std::list<std::vector<int>* > strip_list;

  // Strips of the triangles [t0, t1); returns their number.
  auto strip_range = [&](Int_t t0, Int_t t1) -> Int_t
  {
    ACTCData *tc = actcNew();
    if (tc == 0) throw(_eh + "failed to allocate TC structure.");
    // actcParami(tc, ACTC_OUT_MIN_FAN_VERTS, is maxint);
    actcParami(tc, ACTC_OUT_MAX_PRIM_VERTS, max_verts);
    actcParami(tc, ACTC_OUT_HONOR_WINDING, 1);
    actcBeginInput(tc);
    for (Int_t t = t0; t < t1; ++t)
    {
      Int_t* T = Triangle(t);
      actcAddTriangle(tc, T[0], T[1], T[2]);
    }
    actcEndInput(tc);

    actcBeginOutput(tc);
    int v1, v2, v3;
    Int_t n = 0;
    while (actcStartNextPrim(tc, &v1, &v2) != ACTC_DATABASE_EMPTY)
    {
      int cntp = 2;
      std::vector<int>* vecp = new std::vector<int>;
      vecp->push_back(v1); vecp->push_back(v2);
      while (actcGetNextVert(tc, &v3) != ACTC_PRIM_COMPLETE)
      {
        vecp->push_back(v3);
        ++cntp;
      }
      strip_list.push_back(vecp);
      cnt += cntp;
      ++n;
    }
    actcEndOutput(tc);

    actcDelete(tc);
    return n;
  };

  if (mNStripEls || mNStrips) DeleteTriangleStrips();

  if (HasPatches())
  {
    Int_t first = 0;
    for (Patch& p : mPatches)
    {
      p.fFirstStrip = first;
      p.fNStrips    = p.fNT > 0 ? strip_range(p.fFirstT, p.fFirstT + p.fNT) : 0;
      first        += p.fNStrips;
    }
  }
  else
  {
    strip_range(0, mNTrings);
  }

  mNStripEls = cnt;
  mNStrips   = strip_list.size();

  mStripEls    = new Int_t [mNStripEls];
  mStripTrings = new Int_t [mNStripEls];
  mStripBegs   = new Int_t*[mNStrips];
  mStripLens   = new Int_t [mNStrips];

  Int_t       idx = 0;
  Int_t strip_idx = 0;
  while (!strip_list.empty())
  {
    std::vector<int>* vecp = strip_list.front();
    Int_t s_len = vecp->size();
    mStripBegs[strip_idx] = &(mStripEls[idx]);
    mStripLens[strip_idx] = s_len;

    for (Int_t i=0; i<s_len; ++i, ++idx)
    {
      mStripEls[idx] = (*vecp)[i];

      if (i > 1) {
	xx_tring xx(mStripEls[idx-2], mStripEls[idx-1], mStripEls[idx]);
	std::unordered_map<xx_tring, Int_t>::iterator xi;
	xi = tring_map.find(xx);
	if (xi != tring_map.end()) {
	  mStripTrings[idx] = xi->second;
	} else {
	  printf("%sSafr: %zu %d.\n", _eh.Data(), strip_list.size(), i);
	}
      }

    }

    strip_list.pop_front();
    delete vecp;
    ++strip_idx;
  }
}

void TringTvor::DeleteTriangleStrips()
{
  for (Patch& p : mPatches) p.fFirstStrip = p.fNStrips = 0;
  mNStripEls = 0;
  delete [] mStripEls;    mStripEls  = 0;
  delete [] mStripTrings; mStripTrings = 0;
  mNStrips = 0;
  delete [] mStripBegs;   mStripBegs = 0;
  delete [] mStripLens;   mStripLens = 0;
}

/**************************************************************************/
// Patches
/**************************************************************************/

namespace {

// The rows of v, stride values each, in the order of old_of_new.
template <typename T>
void permute_rows(std::vector<T>& v, Int_t stride, const std::vector<Int_t>& old_of_new)
{
  if (v.empty()) return;
  std::vector<T> r(v.size());
  for (size_t i = 0; i < old_of_new.size(); ++i)
    std::copy_n(&v[stride * size_t(old_of_new[i])], stride, &r[stride * i]);
  v.swap(r);
}

// The vertex range, box and normal cone of the triangles of p; the cone holds
// the normals by the winding, (v1 - v0) x (v2 - v0), of non-degenerate ones.
template <typename P>
void fill_bounds(TringTvor& tt, P& p)
{
  p.fConeAxis[0] = p.fConeAxis[1] = 0; p.fConeAxis[2] = 1;
  p.fConeSin = 1;
  if (p.fNT == 0)
  {
    p.fMinV = 0; p.fMaxV = -1;
    for (Int_t i = 0; i < 6; ++i) p.fCtrExt[i] = 0;
    return;
  }

  Float_t m[3], M[3];
  Double_t sum[3] = { 0, 0, 0 };
  p.fMinV = p.fMaxV = tt.Triangle(p.fFirstT)[0];
  for (Int_t k = 0; k < 3; ++k) m[k] = M[k] = tt.Vertex(p.fMinV)[k];
  for (Int_t t = p.fFirstT; t < p.fFirstT + p.fNT; ++t)
  {
    const Int_t* T = tt.Triangle(t);
    for (Int_t j = 0; j < 3; ++j)
    {
      p.fMinV = std::min(p.fMinV, T[j]);
      p.fMaxV = std::max(p.fMaxV, T[j]);
      const Float_t* V = tt.Vertex(T[j]);
      for (Int_t k = 0; k < 3; ++k) { m[k] = std::min(m[k], V[k]); M[k] = std::max(M[k], V[k]); }
    }
    // Twice the area times the unit normal, so the mean is area-weighted.
    Float_t n[3];
    const Float_t len = tt.CalculateTriangleNormal(t, n);
    if (len > 0)
      for (Int_t k = 0; k < 3; ++k) sum[k] += len * n[k];
  }
  for (Int_t k = 0; k < 3; ++k)
  {
    p.fCtrExt[k]     = 0.5f * (M[k] + m[k]);
    p.fCtrExt[k + 3] = 0.5f * (M[k] - m[k]);
  }

  const Double_t slen = std::sqrt(sum[0]*sum[0] + sum[1]*sum[1] + sum[2]*sum[2]);
  if (slen <= 0) return;
  Double_t axis[3] = { sum[0] / slen, sum[1] / slen, sum[2] / slen };
  Double_t min_cos = 1;
  for (Int_t t = p.fFirstT; t < p.fFirstT + p.fNT; ++t)
  {
    Float_t n[3];
    if (tt.CalculateTriangleNormal(t, n) > 0)
      min_cos = std::min(min_cos, axis[0]*n[0] + axis[1]*n[1] + axis[2]*n[2]);
  }
  for (Int_t k = 0; k < 3; ++k) p.fConeAxis[k] = axis[k];
  // A margin for the rounding of the float normals.
  if (min_cos > 1e-4)
    p.fConeSin = std::min(1.0, std::sqrt(std::max(0.0, 1 - min_cos * min_cos)) + 1e-4);
}

} // namespace

void TringTvor::MakePatches(const std::vector<Int_t>& tring_patch, Int_t n_patches,
                            const std::vector<PatchNode>* tree)
{
  static const Exc_t _eh("TringTvor::MakePatches ");

  if ((Int_t) tring_patch.size() != mNTrings)
    throw _eh + "tring_patch must hold a patch for each triangle.";
  if (n_patches < 1)
    throw _eh + "n_patches must be positive.";
  for (Int_t p : tring_patch)
    if (p < 0 || p >= n_patches) throw _eh + "a patch index is out of range.";
  if (tree)
  {
    const Int_t nn = tree->size();
    for (const PatchNode& n : *tree)
    {
      if (n.fFirstPatch < 0 || n.fNPatches < 1 || n.fFirstPatch + n.fNPatches > n_patches)
        throw _eh + "a node's patches are out of range.";
      for (Int_t c : n.fChild)
        if (c < -1 || c >= nn) throw _eh + "a node's child is out of range.";
    }
  }

  // The triangles by patch, in their order within a patch.
  std::vector<Int_t> first(n_patches + 1, 0);
  for (Int_t p : tring_patch) ++first[p + 1];
  for (Int_t p = 0; p < n_patches; ++p) first[p + 1] += first[p];
  std::vector<Int_t> old_of_new(mNTrings);
  {
    std::vector<Int_t> pos(first.begin(), first.end() - 1);
    for (Int_t t = 0; t < mNTrings; ++t) old_of_new[pos[tring_patch[t]]++] = t;
  }
  permute_rows(mTrings,     3, old_of_new);
  permute_rows(mTringNorms, 3, old_of_new);
  permute_rows(mTringCols,  4, old_of_new);
  permute_rows(mTringTexs,  6, old_of_new);

  // The vertices in the order the triangles first use them; unused ones last.
  std::vector<Int_t> new_v(mNVerts, -1), old_v;
  old_v.reserve(mNVerts);
  for (Int_t& v : mTrings)
  {
    if (new_v[v] < 0) { new_v[v] = old_v.size(); old_v.push_back(v); }
    v = new_v[v];
  }
  for (Int_t v = 0; v < mNVerts; ++v)
    if (new_v[v] < 0) { new_v[v] = old_v.size(); old_v.push_back(v); }
  permute_rows(mVerts, 3, old_v);
  permute_rows(mNorms, 3, old_v);
  permute_rows(mCols,  4, old_v);
  permute_rows(mTexs,  2, old_v);

  DeleteTriangleStrips();

  mPatches.assign(n_patches, Patch());
  for (Int_t p = 0; p < n_patches; ++p)
  {
    Patch& P = mPatches[p];
    P.fFirstT = first[p];
    P.fNT     = first[p + 1] - first[p];
    P.fFirstStrip = P.fNStrips = 0;
    fill_bounds(*this, P);
  }

  mPatchNodes.clear();
  if (tree)
  {
    mPatchNodes = *tree;
    for (PatchNode& n : mPatchNodes)
    {
      n.fFirstT = first[n.fFirstPatch];
      n.fNT     = first[n.fFirstPatch + n.fNPatches] - n.fFirstT;
      fill_bounds(*this, n);
    }
  }
  mPatchStamp = ++s_patch_stamp;
}

void TringTvor::WipePatches()
{
  std::vector<Patch>     p; mPatches.swap(p);
  std::vector<PatchNode> n; mPatchNodes.swap(n);
  mPatchStamp = ++s_patch_stamp;
}

/**************************************************************************/
// Export
/**************************************************************************/

namespace {
template<typename T> void dump3vec(std::ostream& o, T v) {
  o << "< " << v[0] << ", " << v[1] << ", " << v[2] << " >, \n";
}
}

void TringTvor::ExportPovMesh(std::ostream& o, Bool_t smoothp)
{
  // Exports triangle-data as POV mesh2 object.
  // By experimentation I would say that smooth triangles do not work properly
  // in pov-3.6.1 (15.10.2006).

  o << "mesh2 {\n";

  {
    o << "  vertex_vectors { " << mNVerts << ",\n";
    Float_t* V = Verts();
    for (Int_t i=0; i<mNVerts; ++i, V+=3) {
      o << "    "; dump3vec<Float_t*>(o, V);
    }
    o << "  }\n";
  }

  if (smoothp) {
    Bool_t no_normals = ! HasNorms();
    if (no_normals) GenerateVertexNormals();
    o << "  normal_vectors { " << mNVerts << ",\n";
    Float_t* N = Norms();
    for (Int_t i=0; i<mNVerts; ++i, N+=3) {
      o << "    "; dump3vec<Float_t*>(o, N);
    }
    o << "  }\n";
    if (no_normals) { WipeNorms(); }
  }

  {
    o << "  face_indices { " << mNTrings << ",\n";
    Int_t* T = Trings();
    for (Int_t i=0; i<mNTrings; ++i, T+=3) {
      o << "    "; dump3vec<Int_t*>(o, T);
    }
    o << "  }\n";
  }

  o << "}\n";
}
