// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "TriangleBisection.h"

#include <gts/Graph.h>

#include <cstdint>
#include <unordered_map>

void gled::GTS::BisectTriangles(const Int_t* trings, Int_t n_trings, Int_t levels,
                                std::vector<Int_t>& tring_part)
{
  static const Exc_t _eh("GTS::BisectTriangles ");

  if (levels < 0) throw _eh + "levels must not be negative.";
  tring_part.assign(n_trings, 0);
  if (levels == 0 || n_trings == 0) return;

  // The dual graph: a node per triangle, an edge between two triangles on an
  // edge; on a non-manifold edge every other triangle joins the first.
  gts::Graph gr;
  gts::NodeSet g(gr);
  std::vector<gts::GNodeId> node(n_trings);
  for (Int_t t = 0; t < n_trings; ++t)
  {
    node[t] = gr.add_node(1.f, gts::Index(t));
    g.add(node[t]);
  }
  std::unordered_map<std::uint64_t, Int_t> first;
  first.reserve(3 * size_t(n_trings) / 2);
  for (Int_t t = 0; t < n_trings; ++t)
  {
    const Int_t* T = trings + 3 * t;
    for (int k = 0; k < 3; ++k)
    {
      std::uint64_t a = UInt_t(T[k]), b = UInt_t(T[(k + 1) % 3]);
      if (a > b) std::swap(a, b);
      auto [it, added] = first.try_emplace((a << 32) | b, t);
      if ( ! added && it->second != t) gr.add_edge(node[it->second], node[t]);
    }
  }

  // The defaults of GTS's example partition.c: tries of the initial bisection,
  // moves without gain in a refinement, nodes of the coarsest graph, imbalance.
  const unsigned ntry = 10, mmax = 50, nmin = 100;
  const float    imbalance = 0.1f;
  const auto parts = gts::recursive_bisection(gr, g, unsigned(levels), ntry, mmax, nmin, imbalance);
  for (size_t p = 0; p < parts.size(); ++p)
    for (gts::GNodeId n : parts[p].nodes())
      tring_part[gr.data(n)] = Int_t(p);
}
