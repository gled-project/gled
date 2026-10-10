// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

//__________________________________________________________________________
// GTSRetriangulator
//
// Interface to GTS surface 'coarsen' and 'refine' functions.
// Out-of-core simplification is also supported. Note that it sometimes
// produces surfaces with holes.
//
// See GTS manual for details.


#include "GTSRetriangulator.h"

#include <GTS/GTS.h>

#include <Gled/GTime.h>

#include <TMath.h>
#include <TSystem.h>

#include <cmath>

using namespace gled;

#include "GTSRetriangulator.c7"

/**************************************************************************/

namespace
{
  const double r60 = 60*TMath::DegToRad();

  // The angle at the vertex of t opposite e, or -1 if t has none.
  double opposite_angle(const gts::Geometrium& g, gts::FaceId t, gts::EdgeId e)
  {
    gts::VertexId v = g.opposite_vertex(t, e);
    if ( ! v.valid())
      return -1;
    const gts::Vec3 p = g.vertex(v).p;
    const gts::Vec3 a = g.vertex(g.edge(e).v[0]).p - p;
    const gts::Vec3 b = g.vertex(g.edge(e).v[1]).p - p;
    return atan2(gts::norm(gts::cross(a, b)), gts::dot(a, b));
  }

  double cost_angle(const gts::Geometrium& g, gts::EdgeId e)
  {
    // Returns smallest cost of all triangles e is in, where cost for each
    // triangle is:
    //   1 if angle from edge to opposite vertex is 60deg;
    //   goes linearly to 0 as the angle falls (rises) to 0deg (180deg).
    // Works well for cost up 0.5, then becomes unstable.

    double cost = 1.0;
    for (gts::FaceId t : g.faces_of(e))
    {
      const double phi = opposite_angle(g, t, e);
      if (phi >= 0)
        cost = TMath::Min(phi <= r60 ? phi / r60 : 1.5 - 0.5 * phi / r60, cost);
    }
    return cost;
  }

}

/**************************************************************************/

/**************************************************************************/

void GTSRetriangulator::_init()
{
  mTarget = 0;

  mStopOpts = SO_Number;
  mStopNumber = 1000;
  mStopCost   = 0.5;

  mCostOpts = CO_Length;
  mVO_VolumeWght   = 0.5;
  mVO_BoundaryWght = 0.5;
  mVO_ShapeWght    = 0;

  mMidvertOpts = MO_Volume;

  mMinAngleDeg = 1;

  mOutOfCoreDelta = 1e-3;

  bMeasureTime = false;
  mRunTime     = 0;
}

/**************************************************************************/

void GTSRetriangulator::Coarsen()
{
  static const Exc_t _eh("GTSRetriangulator::Coarsen ");

  gSystem->SetFPEMask(kDefaultMask); // kAllMask);

  GTSurf* target = *mTarget;
  if (target == 0)
    throw _eh + "Link Target should be set.";
  std::unique_ptr<GTS::Mesh> s(target->CopySurface());
  if ( ! s)
    throw _eh + "Target should have non-null surface.";

  double stop_cost = mStopCost;

  const gts::VolumeOptimizedParams l_vo_params =
    { mVO_VolumeWght, mVO_BoundaryWght, mVO_ShapeWght };

  gts::EdgeCost l_cost_func;
  switch (mCostOpts)
  {
    case CO_Length:
      stop_cost = stop_cost * stop_cost; // edge-length uses square edge length.
      break;
    case CO_Volume:
      l_cost_func = [l_vo_params](const gts::Geometrium& g, gts::EdgeId e)
        { return gts::volume_optimized_cost(g, e, l_vo_params); };
      break;
    case CO_Angle:
      l_cost_func = cost_angle;
      break;
    default:
      throw _eh + "Unknown CostOpts.";
  }

  gts::CoarsenStop l_stop_func;
  switch (mStopOpts)
  {
    case SO_Number:
      l_stop_func = gts::stop_below_edges(mStopNumber);
      break;
    case SO_Cost:
      l_stop_func = gts::stop_above_cost(stop_cost);
      break;
    default:
      throw _eh + "Unknown StopOpts.";
  }

  gts::CollapsePosition l_coarsen_func;
  switch (mMidvertOpts)
  {
    case MO_Midvert:
      break;
    case MO_Volume:
      l_coarsen_func = [l_vo_params](const gts::Geometrium& g, gts::EdgeId e)
        { return gts::volume_optimized_vertex(g, e, l_vo_params); };
      break;
    default:
      throw _eh + "Unknown MidvertOpts.";
  }

  GTime* start_time = 0;
  if (bMeasureTime) start_time = new GTime(GTime::I_Now);

  gts::coarsen(s->surf(), l_stop_func, l_cost_func, l_coarsen_func,
               mMinAngleDeg*TMath::DegToRad());

  if (bMeasureTime) SetRunTime(start_time->TimeUntilNow().ToDouble());

  target->ReplaceSurface(s.release());
}

/**************************************************************************/

void GTSRetriangulator::Refine()
{
  static const Exc_t _eh("GTSRetriangulator::Refine ");

  GTSurf* target = *mTarget;
  if (target == 0)
    throw _eh + "Link Target should be set.";
  std::unique_ptr<GTS::Mesh> s(target->CopySurface());
  if ( ! s)
    throw _eh + "Target should have non-null surface.";

  double stop_cost = mStopCost;

  const gts::VolumeOptimizedParams l_vo_params =
    { mVO_VolumeWght, mVO_BoundaryWght, mVO_ShapeWght };

  gts::EdgeCost l_cost_func;
  switch (mCostOpts)
  {
    case CO_Length:
      stop_cost = - stop_cost * stop_cost; // refine's length cost is -length^2.
      break;
    case CO_Volume:
      l_cost_func = [l_vo_params](const gts::Geometrium& g, gts::EdgeId e)
        { return gts::volume_optimized_cost(g, e, l_vo_params); };
      break;
    case CO_Angle:
    {
      // Only the edges the surface had are costed; new ones cost 1. A split
      // leaves an angle of at least 90deg at the midpoint, so splitting by
      // the angles of new edges would never end.
      const std::size_t first_new = s->geo().edge_capacity();
      l_cost_func = [first_new](const gts::Geometrium& g, gts::EdgeId e)
        { return e.i < first_new ? cost_angle(g, e) : 1.0; };
      break;
    }
    default:
      throw _eh + "Unknown CostOpts.";
  }

  gts::RefineStop l_stop_func;
  switch(mStopOpts)
  {
    case SO_Number:
    {
      const std::size_t max = mStopNumber;
      l_stop_func = [max](double, std::size_t number) { return number > max; };
      break;
    }
    case SO_Cost:
      // Edges come cheapest first; stop when the cheapest exceeds the limit.
      l_stop_func = [stop_cost](double cost, std::size_t) { return cost > stop_cost; };
      break;
    default:
      throw _eh + "Unknown StopOpts.";
  }

  GTime* start_time = 0;
  if (bMeasureTime) start_time = new GTime(GTime::I_Now);

  gts::refine(s->surf(), l_stop_func, l_cost_func);

  if (bMeasureTime) SetRunTime(start_time->TimeUntilNow().ToDouble());

  target->ReplaceSurface(s.release());
}

//==============================================================================

void GTSRetriangulator::OutOfCoreSimplification()
{
  static const Exc_t _eh("GTSRetriangulator::OutOfCoreSimplification ");

  GTSurf* target = *mTarget;
  if (target == 0)
    throw _eh + "Link Target should be set.";
  std::unique_ptr<GTS::Mesh> s(target->CopySurface());
  if ( ! s)
    throw _eh + "Target should have non-null surface.";

  GTime* start_time = 0;
  if (bMeasureTime) start_time = new GTime(GTime::I_Now);

  auto d = std::make_unique<GTS::Mesh>();

  gts::BBox bbox;
  for (gts::VertexId v : s->surf().vertices())
    bbox.add(s->geo().vertex(v).p);

  printf("Boundingbox is (%f,%f,%f)-(%f,%f,%f)\n", bbox.min.x, bbox.min.y, bbox.min.z, bbox.max.x, bbox.max.y, bbox.max.z);

  gts::Range c_stats;
  try
  {
    gts::ClusterGrid c_grid(d->surf(), bbox, mOutOfCoreDelta);
    for (gts::FaceId f : s->surf().faces())
    {
      const gts::Triangle t = s->geo().triangle(f);
      c_grid.add_triangle(t.a, t.b, t.c);
    }
    c_stats = c_grid.update();
  }
  catch (const std::exception& e)
  {
    throw _eh + e.what();
  }
  s.reset();

  printf ("%d clusters of size: min: %g avg: %.1f|%.1f max: %g\n",
	   c_stats.n, c_stats.min, c_stats.mean, c_stats.stddev, c_stats.max);

  if (bMeasureTime) SetRunTime(start_time->TimeUntilNow().ToDouble());

  target->ReplaceSurface(d.release());
}
