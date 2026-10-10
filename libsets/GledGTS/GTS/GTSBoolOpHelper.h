// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef GTS_GTSBoolOpHelper_H
#define GTS_GTSBoolOpHelper_H


#include "Gled/GledTypes.h"
#include "GTS.h"

#include <set>

namespace gled {

class GTSurf;

namespace GTS
{
  // Copies of a and b, their intersection, the pieces and the result all
  // live in one Geometrium, which TakeResult() hands over.
  class BoolOpHelper
  {
    GTSurf                             *target;
    std::unique_ptr<gts::Geometrium>    geo;
    gts::Surface                       *a_surf;
    gts::Surface                       *b_surf;
    gts::Surface                       *result;
    std::unique_ptr<gts::SurfaceInter>  inter;
    double                              eps_a, eps_p, eps_l;

    int                                 debug; // not used yet

    std::set<gts::EdgeId>               edge_set;

    gts::Surface* import(GTSurf* src, const Exc_t& _eh, const char* which);
    gts::FaceId   other_face(gts::EdgeId e, gts::FaceId t) const;

    // ----------------------------------------------------------------
    // epsi triangles -- area < eps_a, perimeter < eps_p

    bool is_epsi(gts::FaceId t) const;

    void collapse_adjacent_epsi_triangles();

    // ----------------------------------------------------------------
    // zeta - triangles -- area < eps_a, perimeter >= eps_p

    bool is_zeta(gts::FaceId t) const;

    void handle_zeta_triangles();

  public:

    BoolOpHelper(GTSurf* tgt, GTSurf* a, GTSurf* b, const Exc_t& _eh);
    ~BoolOpHelper();

    void set_debug(int d) { debug = d; }

    void BuildInter(const Exc_t& _eh);

    void PostProcess();

    void MakeMerge();
    void MakeUnion();
    void MakeIntersection();
    void MakeDifference();

    Mesh* TakeResult();
  };

}

} // endnamespace gled


#endif
