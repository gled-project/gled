// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "GTSBoolOpHelper.h"
#include "Glasses/GTSurf.h"

#include <TMath.h>

using namespace gled;


//==============================================================================
//==============================================================================

namespace
{
  //----------------------------------------------------------------------------
  // General utilities
  //----------------------------------------------------------------------------

  double edge_len(const gts::Geometrium& g, gts::EdgeId e)
  {
    const gts::Segment s = g.segment(e);
    return gts::distance(s.a, s.b);
  }

  gts::EdgeId longest_edge(const gts::Geometrium& g, gts::FaceId t)
  {
    const auto& e = g.face(t).e;
    double l1 = edge_len(g, e[0]);
    double l2 = edge_len(g, e[1]);
    double l3 = edge_len(g, e[2]);

    if (l1 >= l2 && l1 >= l3) return e[0];
    return (l2 >= l3) ? e[1] : e[2];
  }

  gts::EdgeId shortest_edge(const gts::Geometrium& g, gts::FaceId t)
  {
    const auto& e = g.face(t).e;
    double l1 = edge_len(g, e[0]);
    double l2 = edge_len(g, e[1]);
    double l3 = edge_len(g, e[2]);

    if (l1 <= l2 && l1 <= l3) return e[0];
    return (l2 <= l3) ? e[1] : e[2];
  }

  // The middle of e, as 0.5 (a + b).
  gts::Vec3 mid_point(const gts::Geometrium& g, gts::EdgeId e)
  {
    const gts::Segment s = g.segment(e);
    return { 0.5*(s.a.x + s.b.x), 0.5*(s.a.y + s.b.y), 0.5*(s.a.z + s.b.z) };
  }

  //----------------------------------------------------------------------------
  // Print / debug functions
  //----------------------------------------------------------------------------

  void print_triangles(const gts::Geometrium& g, gts::EdgeId e, gts::FaceId nt)
  {
    int i = 0;
    for (gts::FaceId t : g.faces_of(e))
    {
      printf("      %d. a=%g, p=%g, q=%g", ++i, g.area(t), g.perimeter(t), g.quality(t));

      if (t == nt)
	printf(" *** ");
      printf("\n");
    }
  }
}

//==============================================================================
//==============================================================================

using namespace gled::GTS;

BoolOpHelper::BoolOpHelper(GTSurf* tgt, GTSurf* a, GTSurf* b, const Exc_t& _eh) :
  target(tgt), geo(std::make_unique<gts::Geometrium>()),
  a_surf(0), b_surf(0), result(0),
  eps_a(0), eps_p(0), eps_l(0),
  debug(1)
{
  if (a == b) throw _eh + "Same value of argument a and b.";

  if (a)
  {
    a_surf = import(a, _eh, "a");
  }

  if (b)
  {
    b_surf = import(b, _eh, "b");
  }
}

BoolOpHelper::~BoolOpHelper()
{}

gts::Surface* BoolOpHelper::import(GTSurf* src, const Exc_t& _eh, const char* which)
{
  gts::Surface* s;
  {
    GLensReadHolder _lck(src);
    Mesh* m = src->GetMesh();
    if (m == 0) throw _eh + GForm("Argument %s has null surface.", which);
    s = &geo->import(m->geo(), m->surf());
  }

  if (src != target)
  {
    std::unique_ptr<ZTrans> from(ZNode::BtoA(target, src));
    if (*from == 0) throw _eh + GForm("No common parent with %s.", which);
    TransformSurfaceVertices(*s, *from);
  }
  return s;
}

// The face of result other than t on e, when e has exactly two.
gts::FaceId BoolOpHelper::other_face(gts::EdgeId e, gts::FaceId t) const
{
  gts::FaceId other;
  int n = 0;
  for (gts::FaceId f : geo->faces_of(e))
  {
    if ( ! result->contains(f)) continue;
    ++n;
    if (f != t) other = f;
  }
  return n == 2 ? other : gts::FaceId();
}

//------------------------------------------------------------------------------

void BoolOpHelper::BuildInter(const Exc_t& _eh)
{
  if (a_surf == 0) throw _eh + "Argument a is null.";
  if (b_surf == 0) throw _eh + "Argument b is null.";

  switch (target->GetPostBoolOp())
  {
    case GTSurf::PBM_Noop:
    {
      break;
    }
    case GTSurf::PBM_AsValues:
    {
      eps_a = target->GetPostBoolArea();
      eps_p = target->GetPostBoolPerimeter();
      eps_l = target->GetPostBoolLength();
      break;
    }
    case GTSurf::PBM_AsFractions:
    {
      const gts::QualityStats a_stats = gts::quality_stats(*a_surf);
      const gts::QualityStats b_stats = gts::quality_stats(*b_surf);
      Double_t ma = TMath::Min(a_stats.face_area.min, b_stats.face_area.min);
      Double_t ml = TMath::Min(a_stats.edge_length.min, b_stats.edge_length.min);
      Double_t mp = 3.0 * ml;
      eps_a = ma * target->GetPostBoolArea();
      eps_p = mp * target->GetPostBoolPerimeter();
      eps_l = ml * target->GetPostBoolLength();
      break;
    }
  }

  try
  {
    inter = std::make_unique<gts::SurfaceInter>(*a_surf, *b_surf);
  }
  catch (const std::exception& e)
  {
    throw _eh + e.what();
  }

  const gts::SurfaceInter::Check check = inter->check();
  if (!check.ok)     throw _eh + "Intersection curve not orientable.";
  if (!check.closed) throw _eh + "Intersection curve not closed.";
}

//------------------------------------------------------------------------------

void BoolOpHelper::PostProcess()
{
  if (eps_a == 0 || eps_p == 0)
  {
    if (debug > 0)
      printf("BoolOpHelper::PostProcess Limits zero ... nothing to be done.\n");
    return;
  }

  if (debug > 0)
    printf("BoolOpHelper::PostProcess Entering.\n");

  // Only the faces of the result remain.
  inter.reset();
  if (a_surf) { geo->remove_surface(*a_surf); a_surf = 0; }
  if (b_surf) { geo->remove_surface(*b_surf); b_surf = 0; }

  collapse_adjacent_epsi_triangles();

  handle_zeta_triangles();

  for (gts::EdgeId e : edge_set)
  {
    if ( ! geo->dead(e) && edge_len(*geo, e) < eps_l)
      geo->collapse_edge(e, mid_point(*geo, e));
  }

  if (debug > 0)
      printf("BoolOpHelper::PostProcess Done.\n");
}

//------------------------------------------------------------------------------

void BoolOpHelper::MakeMerge()
{
  result = &geo->new_surface();
  if (a_surf) result->merge(*a_surf);
  if (b_surf) result->merge(*b_surf);
}

void BoolOpHelper::MakeUnion()
{
  result = &geo->new_surface();

  inter->boolean(*result, gts::BooleanOp::A_out_B);
  inter->boolean(*result, gts::BooleanOp::B_out_A);

  if (target->GetPostBoolOp() != GTSurf::PBM_Noop)
    PostProcess();
}

void BoolOpHelper::MakeIntersection()
{
  result = &geo->new_surface();

  inter->boolean(*result, gts::BooleanOp::A_in_B);
  inter->boolean(*result, gts::BooleanOp::B_in_A);

  if (target->GetPostBoolOp() != GTSurf::PBM_Noop)
    PostProcess();
}

void BoolOpHelper::MakeDifference()
{
  result = &geo->new_surface();

  inter->boolean(*result, gts::BooleanOp::A_out_B);
  gts::Surface& b_in_a = geo->new_surface();
  inter->boolean(b_in_a, gts::BooleanOp::B_in_A);
  InvertSurface(b_in_a);
  result->merge(b_in_a);
  geo->remove_surface(b_in_a);

  if (target->GetPostBoolOp() != GTSurf::PBM_Noop)
    PostProcess();
}

//------------------------------------------------------------------------------

Mesh* BoolOpHelper::TakeResult()
{
  if (result == 0) return 0;

  inter.reset();
  if (a_surf) { geo->remove_surface(*a_surf); a_surf = 0; }
  if (b_surf) { geo->remove_surface(*b_surf); b_surf = 0; }

  Mesh *ret = new Mesh(std::move(geo), *result);
  result = 0;
  return ret;
}


//==============================================================================
// Internal functions
//==============================================================================

//----------------------------------------------------------------------------
// Removal of a two adjacent triangles, both with area ~ 0, perimeter ~ 0.
// Collapse all edges into a single vertex.
//----------------------------------------------------------------------------

bool BoolOpHelper::is_epsi(gts::FaceId t) const
{
  return geo->area(t) < eps_a && geo->perimeter(t) < eps_p;
}

void BoolOpHelper::collapse_adjacent_epsi_triangles()
{
  const gts::Geometrium& g = *geo;

  auto select = [&](std::set<std::pair<gts::FaceId, gts::FaceId>>& pairs, size_t& n_epsi) {
    pairs.clear(); n_epsi = 0;
    for (gts::FaceId t : result->faces())
    {
      if ( ! is_epsi(t)) continue;
      ++n_epsi;
      for (gts::EdgeId e : g.face(t).e)
      {
        const gts::FaceId ot = other_face(e, t);
        if (ot.valid() && is_epsi(ot))
        {
          pairs.insert(std::minmax(t, ot));
          break;
        }
      }
    }
  };

  std::set<std::pair<gts::FaceId, gts::FaceId>> tripairs;
  size_t n_epsi;
  select(tripairs, n_epsi);

  if (debug > 0)
    printf("Begin epsi pair removal, Nepsi_pair=%zu, Nepsi=%zu\n", tripairs.size(), n_epsi);

  // The two faces, and the faces across their other four edges, go; the four
  // vertices become one at the middle of the common edge.
  for (auto [t1, t2] : tripairs)
  {
    if ( ! result->contains(t1) || ! result->contains(t2)) continue;

    const gts::EdgeId common_edge = g.common_edge(t1, t2);
    if ( ! common_edge.valid()) continue;

    const gts::Vec3     mid = mid_point(g, common_edge);
    const gts::VertexId v3  = g.opposite_vertex(t1, common_edge);
    const gts::VertexId v4  = g.opposite_vertex(t2, common_edge);

    gts::VertexId m = geo->collapse_edge(common_edge, mid);
    for (gts::VertexId v : { v3, v4 })
    {
      if ( ! m.valid() || g.dead(v)) continue;
      const gts::EdgeId e = g.find_edge(m, v);
      if (e.valid()) m = geo->collapse_edge(e, mid);
    }
  }

  if (debug > 0)
  {
    select(tripairs, n_epsi);
    printf("End epsi pair removal, Nepsi_pair=%zu, Nepsi=%zu\n", tripairs.size(), n_epsi);
  }
}


//----------------------------------------------------------------------------
// Handling of triangles with area ~ 0, perimeter > eps.
// First check shortest edge -> if length < eps, collapse shortest edge.
// Otherwise swap the longest edge so that it connects the vertices
// opposite to it.
//----------------------------------------------------------------------------

bool BoolOpHelper::is_zeta(gts::FaceId t) const
{
  return geo->area(t) < eps_a && geo->perimeter(t) >= eps_p;
}

void BoolOpHelper::handle_zeta_triangles()
{
  int n_collapsed = 0, n_reconnected = 0, n_failed = 0;

  const gts::Geometrium& g = *geo;

  std::set<gts::FaceId> tset;
  for (gts::FaceId t : result->faces())
    if (is_zeta(t)) tset.insert(t);

  if (debug > 0)
  {
    printf("Begin zeta handling, Nzeta=%zu\n", tset.size());
  }

  while ( ! tset.empty())
  {
    const gts::FaceId t = *tset.begin();
    tset.erase(tset.begin());
    if ( ! result->contains(t)) continue;

    gts::EdgeId el = longest_edge(g, t);
    gts::EdgeId es = shortest_edge(g, t);

    if (debug > 1)
    {
      printf("a=%22.18g, p=%22.18g, q=%22.18g\n", g.area(t), g.perimeter(t), g.quality(t));
      printf("  longest=%.18g (%d), shortest=%.18g (%d)\n",
	     edge_len(g, el), g.collapsible(el),
	     edge_len(g, es), g.collapsible(es));
      print_triangles(g, el, t);
      print_triangles(g, es, t);
    }

    gts::FaceId to;

    if (edge_len(g, es) > eps_p)
    {
      if (debug > 1)
      {
	printf(" Reconnecting vertices opposite to longest edge.\n");
      }

      to = other_face(el, t);
      try
      {
        gts::swap_edge(*result, el);
      }
      catch (const std::invalid_argument& e)
      {
        if (debug > 1) printf(" Swap failed: %s\n", e.what());
        ++n_failed;
        continue;
      }

      // Collect shortest edges -- these can be safely
      // collapsed after the surface has been processed with this function.
      // Coarsening also removes them.
      // Edge-length limit is applied afterwards.
      edge_set.insert(es);

      ++n_reconnected;
    }
    else
    {
      if (debug > 1)
      {
	printf(" Collapsing shortest edge.\n");
      }

      to = other_face(es, t);

      geo->collapse_edge(es, mid_point(g, es));

      ++n_collapsed;
    }

    if (to.valid()) tset.erase(to);
  }

  if (debug > 0)
  {
    printf("End zeta handling, Ncollapsed=%d, Nreconnected=%d, Nfailed=%d\n", n_collapsed, n_reconnected, n_failed);
  }
}
