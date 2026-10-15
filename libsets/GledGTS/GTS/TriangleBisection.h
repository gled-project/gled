// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef GledGTS_TriangleBisection_H
#define GledGTS_TriangleBisection_H

// Needs no gts-cxx headers, so that other libsets can use it.
#include <Gled/GledTypes.h>

#include <vector>

namespace gled
{
  namespace GTS
  {
    // The part of each triangle among 2^levels, by recursive bisection; the
    // two halves of every split are adjacent in the numbering.
    void BisectTriangles(const Int_t* trings, Int_t n_trings, Int_t levels,
                         std::vector<Int_t>& tring_part);
  }
}

#endif
