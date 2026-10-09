// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ZTrans.h"

using namespace gled;


//______________________________________________________________________
// ZTrans
//
// ZTrans is a 4x4 transformation matrix for homogeneous coordinates
// stored internaly in a column-major order to allow direct usage by
// GL. The element type is Double_t: continuous operations on the matrix
// must retain precision of column vectors, and a streamed copy must be
// identical to the original.
//
// Cartan angles in mA[1-3] (+z, -y, +x) are stored for backward
// compatibility and will probably be removed soon.
//
// Direct  element access (first two should be used with care):
// operator[i]    direct access to elements,   i:0->15
// CM(i,j)        element 4*j + i;           i,j:0->3    { CM ~ c-matrix }
// operator(i,j)  element 4*(j-1) + i - 1    i,j:1->4
//
// Column-vector access:
// USet Get/SetBaseVec(), Get/SetPos() and Arr[XYZT]() methods.
//
// For all methods taking the matrix indices:
// 1->X, 2->Y, 3->Z; 4->Position (if applicable). 0 reserved for time.
//
// Shorthands in method-names:
// LF ~ LocalFrame; PF ~ ParentFrame; IP ~ InPlace

/**************************************************************************/

//ZTrans::ZTrans(const Double_t arr[16]) { SetFromArray(arr); }

//ZTrans::ZTrans(const Float_t  arr[16]) { SetFromArray(arr); }

ZTrans ZTrans::operator*(const ZTrans& t)
{
  ZTrans b(*this);
  b.MultRight(t);
  return b;
}
