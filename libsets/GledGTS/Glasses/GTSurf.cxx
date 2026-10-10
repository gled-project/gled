// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "GTSurf.h"
#include <Glasses/LegendreCoefs.h>

#include <Glasses/ZQueen.h>
#include <GTS/GTS.h>
#include <GTS/GTSBoolOpHelper.h>

#include <TMath.h>
#include <TRandom3.h>
#include <TTree.h>

#include <iostream>

using namespace gled;

#include "GTSurf.c7"

//______________________________________________________________________________
//
// Wrapper over GTS surface.


void GTSurf::_init()
{
  // Override settings from ZGlass
  bUseDispList = true;

  mColor.rgba(1, 1, 1, 1);
  mPointColor.rgba(1, 0, 0, 1);
  bRnrPoints = false;

  pMesh = 0;
  mVerts = mEdges = mFaces = 0;

  mPostBoolOp = PBM_AsFractions;
  mPostBoolArea      = 1e-8;
  mPostBoolPerimeter = 1e-8;
  mPostBoolLength    = 1e-1;
}

GTSurf::~GTSurf()
{
  delete pMesh;
}

/**************************************************************************/

void GTSurf::ReplaceSurface(GTS::Mesh* new_mesh)
{
  // Elements left dead by the operation that made the mesh are reclaimed.
  if (new_mesh) new_mesh->geo().collect();

  GLensReadHolder _lck(this);
  delete pMesh;
  pMesh = new_mesh;
  mStampReqTring = Stamp(FID());
}

GTS::Mesh* GTSurf::CopySurface()
{
  GLensReadHolder _lck(this);

  if (pMesh == 0) return 0;

  return pMesh->Copy();
}

GTS::Mesh* GTSurf::DisownSurface()
{
  GLensReadHolder _lck(this);

  GTS::Mesh* m = pMesh;
  pMesh = 0;
  mStampReqTring = Stamp(FID());

  return m;
}

void GTSurf::GetTriangles(std::vector<Double_t>& verts, std::vector<Int_t>& faces)
{
  verts.clear(); faces.clear();
  if (pMesh == 0) return;

  const gts::Geometrium& g = pMesh->geo();
  std::vector<Int_t> index(g.vertex_capacity(), -1);
  Int_t n = 0;
  for (gts::VertexId v : pMesh->surf().vertices())
  {
    index[v.i] = n++;
    const gts::Vec3& p = g.vertex(v).p;
    verts.insert(verts.end(), { p.x, p.y, p.z });
  }
  for (gts::FaceId f : pMesh->surf().faces())
    for (gts::VertexId v : g.vertices_of(f))
      faces.push_back(index[v.i]);
}

void GTSurf::SetTriangles(const std::vector<Double_t>& verts, const std::vector<Int_t>& faces)
{
  static const Exc_t _eh("GTSurf::SetTriangles ");

  auto m = std::make_unique<GTS::Mesh>();
  gts::Geometrium& g = m->geo();
  const Int_t nv = verts.size() / 3;
  for (Int_t i = 0; i < nv; ++i)
    g.add_vertex({ verts[3*i], verts[3*i + 1], verts[3*i + 2] });
  auto edge = [&](Int_t a, Int_t b) {
    if (a < 0 || a >= nv || b < 0 || b >= nv)
      throw _eh + GForm("vertex index out of range in edge (%d, %d).", a, b);
    const gts::VertexId va(a), vb(b);
    const gts::EdgeId e = g.find_edge(va, vb);
    return e.valid() ? e : g.add_edge(va, vb);
  };
  try
  {
    for (size_t i = 0; i + 2 < faces.size(); i += 3)
    {
      const Int_t* t = &faces[i];
      m->surf().add_face(g.add_face(edge(t[0], t[1]), edge(t[1], t[2]), edge(t[2], t[0])));
    }
  }
  catch (const std::exception& e)
  {
    throw _eh + e.what();
  }
  ReplaceSurface(m.release());
}


//==============================================================================

namespace
{
  struct projected_area_sum_arg
  {
    HPointD dir;
    double  sum;

    projected_area_sum_arg(double x, double y, double z) : dir(x,y,z), sum(0) {}
  };

  void projected_area_sum(const gts::Geometrium& g, gts::FaceId f, projected_area_sum_arg* arg)
  {
    // This is summing *twice* the area of each triangle.

    const gts::Vec3 n = g.normal(f);
    HPointD p(n.x, n.y, n.z);
    arg->sum += TMath::Abs(arg->dir.Dot(p));
  }
}

Double_t GTSurf::GetArea() const
{
  if (!pMesh) return 0;
  return gts::area(pMesh->surf());
}

Double_t GTSurf::GetXYArea() const
{
  if (!pMesh) return 0;

  projected_area_sum_arg arg(0, 0, 1);
  for (gts::FaceId f : pMesh->surf().faces())
    projected_area_sum(pMesh->geo(), f, &arg);
  return arg.sum / 4;
}

Double_t GTSurf::GetVolume() const
{
  if (!pMesh) return 0;
  return gts::volume(pMesh->surf());
}


//==============================================================================

void GTSurf::Load(const TString& file)
{
  static const Exc_t _eh("GTSurf::Load ");

  TString file_name = file.IsNull() ? mFile : file;

  auto g = std::make_unique<gts::Geometrium>();
  auto s = gts::read_gts(*g, std::filesystem::path(file_name.Data()));
  if ( ! s)
  {
    ISerr(_eh + GForm("Reading '%s' failed at line %u, position %u: %s", file_name.Data(),
                      s.error().line, s.error().pos, s.error().message.c_str()));
    return;
  }

  ReplaceSurface(new GTS::Mesh(std::move(g), **s));
}

void GTSurf::Save(const TString& file)
{
  static const Exc_t _eh("GTSurf::Save ");

  if (pMesh == 0) {
    ISerr(_eh + "Surface is null.");
    return;
  }

  TString file_name = file.IsNull() ? mFile : file;

  if ( ! gts::write_gts(pMesh->surf(), std::filesystem::path(file_name.Data())))
    ISerr(_eh + GForm("Can not open file '%s'.", file_name.Data()));
}


//==============================================================================

namespace
{
  void copy_stats(SRange& d, const gts::Range& s)
  {
    d.SetMin(s.min);  d.SetMax(s.max);
    d.SetSumX(s.sum); d.SetSumX2(s.sum2);
    d.SetN(s.n);
  }
}

void GTSurf::CalcStats()
{
  if (pMesh == 0) return;

  const gts::Surface& s = pMesh->surf();
  mVerts = s.vertex_number();
  mEdges = s.edge_number();
  mFaces = s.face_number();

  const gts::QualityStats stats = gts::quality_stats(s);
  copy_stats(mFaceQuality, stats.face_quality);
  copy_stats(mFaceArea,    stats.face_area);
  copy_stats(mEdgeLength,  stats.edge_length);
  copy_stats(mEdgeAngle,   stats.edge_angle);

  Stamp(FID());
}

void GTSurf::PrintStats()
{
  if (pMesh) {
    std::cout.flush(); fflush(stdout);
    gts::print_stats(pMesh->surf(), std::cout);
    std::cout.flush();
  }
}

/**************************************************************************/

void GTSurf::Destroy()
{
  if (pMesh == 0) return;

  delete pMesh;
  pMesh = 0;
  mStampReqTring = Stamp(FID());
}

void GTSurf::Invert()
{
  if (pMesh)
  {
    InvertSurface(pMesh->surf());
    mStampReqTring = Stamp(FID());
  }
}

/**************************************************************************/

namespace
{
  void scale_vertices(GTS::Mesh& m, const Double_t* s)
  {
    for (gts::VertexId v : m.surf().vertices())
    {
      gts::Vec3& p = m.geo().position(v);
      p.x *= s[0]; p.y *= s[1]; p.z *= s[2];
    }
  }
}

void GTSurf::Rescale(Double_t s)
{
  if (pMesh)
  {
    Double_t sxyz[3] = { s, s, s };
    scale_vertices(*pMesh, sxyz);
    mStampReqTring = Stamp(FID());
  }
}

void GTSurf::RescaleXYZ(Double_t sx, Double_t sy, Double_t sz)
{
  if (pMesh)
  {
    Double_t sxyz[3] = { sx, sy, sz };
    scale_vertices(*pMesh, sxyz);
    mStampReqTring = Stamp(FID());
  }
}

void GTSurf::TransformAndResetTrans()
{
  if (pMesh)
    TransformSurfaceVertices(pMesh->surf(), mTrans);
  UnitTrans();
}

void GTSurf::RotateAndResetRot()
{
  if (pMesh)
    RotateSurfaceVertices(pMesh->surf(), mTrans);
  UnitRot();
}


//==============================================================================

namespace
{
  // At a + 0.5 (b - a), not at gts::midvertex's (a + b) / 2.
  gts::VertexId mid_edge_splitter(gts::Geometrium& g, gts::EdgeId e)
  {
    return g.interpolate_vertex(g.edge(e).v[0], g.edge(e).v[1], 0.5);
  }
}

void GTSurf::Tessellate(UInt_t order, Bool_t mid_edge)
{
  if (pMesh == 0) return;

  while (order--)
  {
    gts::tessellate(pMesh->surf(), mid_edge ? gts::Splitter(mid_edge_splitter) : gts::Splitter());
  }
  pMesh->geo().collect();
  mStampReqTring = Stamp(FID());
}

//==============================================================================

void GTSurf::Merge(GTSurf* a, GTSurf* b)
{
  // Merge surfaces a and b into current surface.
  // a or b can be null.
  // Vertices of a and b are transformed into local coordinate system.

  static const Exc_t _eh("GTSurf::Merge ");

  GTS::BoolOpHelper boh(this, a, b, _eh);
  boh.MakeMerge();
  ReplaceSurface(boh.TakeResult());
}

void GTSurf::Union(GTSurf* a, GTSurf* b)
{
  // Merge union of a and b into this surface.
  // Vertices of a and b are transformed into local coordinate system.

  static const Exc_t _eh("GTSurf::Union ");

  GTS::BoolOpHelper boh(this, a, b, _eh);
  boh.BuildInter(_eh);
  boh.MakeUnion();
  ReplaceSurface(boh.TakeResult());
}

void GTSurf::Intersection(GTSurf* a, GTSurf* b)
{
  // Merge intersection of a and b into this surface.
  // Vertices of a and b are transformed into local coordinate system.

  static const Exc_t _eh("GTSurf::Intersection ");

  GTS::BoolOpHelper boh(this, a, b, _eh);
  boh.BuildInter(_eh);
  boh.MakeIntersection();
  ReplaceSurface(boh.TakeResult());
}

void GTSurf::Difference(GTSurf* a, GTSurf* b)
{
  // Merge difference of a and b into this surface.
  // Vertices of a and b are transformed into local coordinate system.

  static const Exc_t _eh("GTSurf::Difference ");

  GTS::BoolOpHelper boh(this, a, b, _eh);
  boh.BuildInter(_eh);
  boh.MakeDifference();
  ReplaceSurface(boh.TakeResult());
}

//==============================================================================

void GTSurf::GenerateSphere(UInt_t order)
{
  static const Exc_t _eh("GTSurf::GenerateSphere ");

  if (order == 0) throw _eh + "order must be at least 1.";

  auto m = std::make_unique<GTS::Mesh>();
  gts::generate_sphere(m->surf(), order);
  ReplaceSurface(m.release());
}

void GTSurf::GenerateTriangle(Double_t s)
{
  auto m = std::make_unique<GTS::Mesh>();
  gts::Geometrium& g = m->geo();

  const Double_t sqrt3 = TMath::Sqrt(3);
  gts::VertexId v[3];
  v[0] = g.add_vertex({ -s*0.5, -s*sqrt3/6, 0 });
  v[1] = g.add_vertex({  s*0.5, -s*sqrt3/6, 0 });
  v[2] = g.add_vertex({  0,      s*sqrt3/3, 0 });

  gts::EdgeId e[3];
  e[0] = g.add_edge(v[0], v[1]);
  e[1] = g.add_edge(v[1], v[2]);
  e[2] = g.add_edge(v[2], v[0]);

  m->surf().add_face(g.add_face(e[0], e[1], e[2]));
  ReplaceSurface(m.release());
}

namespace
{
  // Adds the vertices of m to me; verts gets them in the order of adding,
  // which is the order of the values in me.
  void lcme_fill(GTS::Mesh& m, LegendreCoefs::MultiEval& me, std::vector<gts::VertexId>& verts)
  {
    verts.clear();
    for (gts::VertexId v : m.surf().vertices()) verts.push_back(v);
    me.Init(verts.size());
    for (gts::VertexId v : verts)
    {
      const gts::Vec3& p = m.geo().vertex(v).p;
      me.AddPoint(p.x, p.y, p.z, 0);
    }
    me.Sort();
  }
}

void GTSurf::GenerateSphereThetaConst(UInt_t order)
{
  auto m = std::make_unique<GTS::Mesh>();
  gts::generate_sphere(m->surf(), 1);
  std::vector<gts::VertexId> verts;
  Double_t quater_len = 1.051462 / 4;

  for (UInt_t cgo = 1; cgo < order; ++cgo)
  {
    gts::tessellate(m->surf());

    LegendreCoefs::MultiEval me;
    lcme_fill(*m, me, verts);

    quater_len *= 0.5;

    Double_t dtheta_max = 2.0 * TMath::ATan(quater_len);
    Double_t theta0 = TMath::ACos(me.fMVec[me.fIdcs[0]]);
    Double_t z_sum  = me.fMVec[me.fIdcs[0]];
    Int_t    i0 = 0, i = 1;

    while (i < me.fN)
    {
      Int_t    ii    = me.fIdcs[i];
      Double_t theta = TMath::ACos(me.fMVec[ii]);
      if (i + 1 >= me.fN) ++i;
      if (theta > theta0 + dtheta_max || i >= me.fN)
      {
	if (i - i0 > 1)
	{
	  Double_t z = z_sum / (i - i0);
	  for (Int_t j = i0; j < i; ++j)
	  {
	    Int_t jj = me.fIdcs[j];
	    gts::Vec3 &p = m->geo().position(verts[jj]);
	    p.z = me.fPhis[jj] > 0 ? z : -z;
	    Double_t fac = p.x*p.x + p.y*p.y;
	    if (fac > 1e-18)
	    {
	      fac = sqrt((1.0 - z*z) / fac);
	      p.x *= fac;
	      p.y *= fac;
	    }
	    else
	    {
	      p.z = p.z > 0 ? 1.0 : -1.0;
	    }
	  }
	}
	theta0 = theta;
	z_sum  = me.fMVec[ii];
	i0     = i;
      }
      else
      {
	z_sum += me.fMVec[ii];
      }
      ++i;
    }
  }

  ReplaceSurface(m.release());
}


//==============================================================================
// Legendrification
//==============================================================================

namespace
{
  void legendre_vertex_adder(gts::Vec3& p, LegendreCoefs::Evaluator* e)
  {
    HPointD  vec(p.x, p.y, p.z);
    vec *= 1.0 + e->Eval(vec) / vec.Mag();
    p.x = vec.x; p.y = vec.y; p.z = vec.z;
  }

  void legendre_vertex_scaler(gts::Vec3& p, LegendreCoefs::Evaluator* e)
  {
    HPointD  vec(p.x, p.y, p.z);
    vec *= 1.0 + e->Eval(vec);
    p.x = vec.x; p.y = vec.y; p.z = vec.z;
  }

  template <class F>
  void foreach_position(GTS::Mesh& m, F f)
  {
    for (gts::VertexId v : m.surf().vertices()) f(m.geo().position(v));
  }
}

void GTSurf::LegendrofyAdd(LegendreCoefs* lc, Double_t scale, Int_t l_max)
{
  static const Exc_t _eh("GTSurf::LegendrofyAdd ");

  if (pMesh == 0) throw _eh + "member pMesh is 0.";
  if (lc    == 0) throw _eh + "argument lc is 0.";

  LegendreCoefs::Evaluator eval(lc, scale, l_max);

  foreach_position(*pMesh, [&](gts::Vec3& p) { legendre_vertex_adder(p, &eval); });

  mStampReqTring = Stamp(FID());
}

void GTSurf::LegendrofyScale(LegendreCoefs* lc, Double_t scale, Int_t l_max)
{
  static const Exc_t _eh("GTSurf::LegendrofyScale ");

  if (pMesh == 0) throw _eh + "member pMesh is 0.";
  if (lc    == 0) throw _eh + "argument lc is 0.";

  LegendreCoefs::Evaluator eval(lc, scale, l_max);

  foreach_position(*pMesh, [&](gts::Vec3& p) { legendre_vertex_scaler(p, &eval); });

  mStampReqTring = Stamp(FID());
}

void GTSurf::LegendrofyScaleRandom(Int_t l_max, Double_t abs_scale, Double_t pow_scale)
{
  // Single-shopping wrapper -- creates a dummy LegendreCoefs lens without
  // enlightening it.

  static const Exc_t _eh("GTSurf::LegendrofyScaleRandom ");

  if (pMesh == 0) throw _eh + "member pMesh is 0.";

  std::unique_ptr<LegendreCoefs> lc(new LegendreCoefs);
  lc->InitRandom(l_max, abs_scale, pow_scale);
  lc->SetCoef(0, 0, 0);

  LegendreCoefs::Evaluator eval(lc.get());

  foreach_position(*pMesh, [&](gts::Vec3& p) { legendre_vertex_scaler(p, &eval); });

  mStampReqTring = Stamp(FID());
}

//------------------------------------------------------------------------------
// Legendrification, the Multi way
//------------------------------------------------------------------------------

namespace
{
  void legendrofy_multi_common(GTS::Mesh* m, LegendreCoefs* lc, LegendreCoefs::MultiEval& me,
                               std::vector<gts::VertexId>& verts, const Exc_t eh)
  {
    if (m  == 0) throw eh + "member pMesh is 0.";
    if (lc == 0) throw eh + "argument lc is 0.";

    lcme_fill(*m, me, verts);
  }
}

void GTSurf::LegendrofyAddMulti(LegendreCoefs* lc, Double_t scale, Int_t l_max)
{
  static const Exc_t _eh("GTSurf::LegendrofyAddMulti ");

  LegendreCoefs::MultiEval me;
  std::vector<gts::VertexId> verts;

  legendrofy_multi_common(pMesh, lc, me, verts, _eh);

  lc->EvalMulti(me, l_max);

  for (Int_t i = 0; i < me.fN; ++i)
  {
    gts::Vec3 &p = pMesh->geo().position(verts[i]);
    HPointD  vec(p.x, p.y, p.z);
    vec *= 1.0 + scale * me.fMVec[i] / vec.Mag();
    p.x = vec.x; p.y = vec.y; p.z = vec.z;
  }

  mStampReqTring = Stamp(FID());
}

void GTSurf::LegendrofyScaleMulti(LegendreCoefs* lc, Double_t scale, Int_t l_max)
{
  static const Exc_t _eh("GTSurf::LegendrofyScaleMulti ");

  LegendreCoefs::MultiEval me;
  std::vector<gts::VertexId> verts;

  legendrofy_multi_common(pMesh, lc, me, verts, _eh);

  lc->EvalMulti(me, l_max);

  for (Int_t i = 0; i < me.fN; ++i)
  {
    gts::Vec3 &p = pMesh->geo().position(verts[i]);
    HPointD  vec(p.x, p.y, p.z);
    vec *= 1.0 + scale * me.fMVec[i];
    p.x = vec.x; p.y = vec.y; p.z = vec.z;
  }

  mStampReqTring = Stamp(FID());
}

void GTSurf::LegendrofyScaleRandomMulti(Int_t l_max, Double_t abs_scale, Double_t pow_scale)
{
  static const Exc_t _eh("GTSurf::LegendrofyRandomMulti ");

  std::unique_ptr<LegendreCoefs> lc(new LegendreCoefs);
  lc->InitRandom(l_max, abs_scale, pow_scale);
  lc->SetCoef(0, 0, 0);

  LegendreCoefs::MultiEval me;
  std::vector<gts::VertexId> verts;

  legendrofy_multi_common(pMesh, lc.get(), me, verts, _eh);

  lc->EvalMulti(me, l_max);

  for (Int_t i = 0; i < me.fN; ++i)
  {
    gts::Vec3 &p = pMesh->geo().position(verts[i]);
    const Double_t fac = 1.0 + me.fMVec[i];
    p.x *= fac; p.y *= fac; p.z *= fac;
  }

  mStampReqTring = Stamp(FID());
}


//==============================================================================
// Triangle exporter
//==============================================================================

void GTSurf::ExportTring(const Text_t* fname)
{
  // Dumps vertices/triangles in a trivial format.

  if (pMesh == 0) return;

  std::vector<Double_t> verts;
  std::vector<Int_t>    faces;
  GetTriangles(verts, faces);

  FILE* f = (fname) ? fopen(fname, "w") : stdout;

  fprintf(f, "%zu %zu\n", verts.size() / 3, faces.size() / 3);
  for (size_t i = 0; i < verts.size(); i += 3)
    fprintf(f, "%lf %lf %lf\n", verts[i], verts[i + 1], verts[i + 2]);
  for (size_t i = 0; i < faces.size(); i += 3)
    fprintf(f, "%d %d %d\n", faces[i], faces[i + 1], faces[i + 2]);

  if (fname) fclose(f);
}


//==============================================================================
// Making of split surfaces
//==============================================================================

void GTSurf::MakeZSplitSurfaces(Double_t z_split, const TString& stem, Bool_t save_p)
{
  static const Exc_t _eh("GTSurf::SaveZSplitSurfaces ");

  GTS::Mesh *sup = 0, *sdn = 0;

  {
    GLensReadHolder _rdlck(this);

    if (!pMesh)
      throw _eh + "Surface is null.";

    sup = CopySurface();
    sdn = CopySurface();
  }

  foreach_position(*sup, [&](gts::Vec3& p) { if (p.z < z_split) p.z = z_split; });
  foreach_position(*sdn, [&](gts::Vec3& p) { if (p.z > z_split) p.z = z_split; });

  GTSurf *gsup = new GTSurf(GForm("Upper %s", GetName()));
  gsup->ReplaceSurface(sup);
  gsup->SetFile(GForm("%s-upper.gts", stem.Data()));

  GTSurf *gsdn = new GTSurf(GForm("Lower %s", GetName()));
  gsdn->ReplaceSurface(sdn);
  gsdn->SetFile(GForm("%s-lower.gts", stem.Data()));

  {
    GLensWriteHolder _wrlck(this);

    mQueen->CheckIn(gsup);
    mQueen->CheckIn(gsdn);

    Add(gsup);
    Add(gsdn);
  }

  if (save_p)
  {
    { GLensReadHolder _rdlck(gsup); gsup->Save(); }
    { GLensReadHolder _rdlck(gsdn); gsdn->Save(); }
  }
}

//==============================================================================

TTree* GTSurf::MakeHPointDTree(const TString& name, const TString& title)
{
  if (pMesh == 0) return 0;

  TTree *t = new TTree(name, title);
  t->SetDirectory(0);

  HPointD *p = 0;
  t->Branch("P", &p);

  foreach_position(*pMesh, [&](gts::Vec3& v) { p->Set(v.x, v.y, v.z); t->Fill(); });

  return t;
}

TTree* GTSurf::MakeMultiEvalTree(const TString& name, const TString& title)
{
  if (pMesh == 0) return 0;

  LegendreCoefs::MultiEval me;
  std::vector<gts::VertexId> verts;
  lcme_fill(*pMesh, me, verts);

  TTree *t = new TTree(name, title);
  t->SetDirectory(0);

  Double_t ct, d;
  t->Branch("B1", &ct, "ct/D");
  t->Branch("B2", &d,  "d/D");

  for (Int_t i = 1; i < me.fN; ++i)
  {
    Int_t i1 = me.fIdcs[i];
    Int_t i0 = me.fIdcs[i - 1];

    ct = me.fMVec[i1];
    d  = me.fMVec[i1] - me.fMVec[i0];
    t->Fill();
  }

  return t;
}
