// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef GledCore_ZTrans_H
#define GledCore_ZTrans_H

#include <Stones/HTrans.h>

namespace gled {


//==============================================================================
// ZPoint
//==============================================================================

class ZPoint : public HPoint<Double_t>
{
  typedef HPoint<Double_t> TP;

public:
  ZPoint() : TP() {}
  ZPoint(const Float_t* p)  : TP(p) {}
  ZPoint(const Double_t* p) : TP(p) {}
  ZPoint(Double_t _x, Double_t _y, Double_t _z) : TP(_x, _y, _z) {}
  template <typename OO>
  ZPoint(const HPoint<OO>& v) : TP(v.x, v.y, v.z) {}

  ClassDefNV(ZPoint, 1); // Simple, streamable 3D point.
};


//==============================================================================
// ZTrans -- 3D transformation in generalised coordinates
//==============================================================================

class ZTrans : public HTrans<Double_t>
{
  typedef HTrans<Double_t> TP;

public:
  ZTrans() : TP() {}
  ZTrans(const ZTrans& z) : TP(z) {}
  //ZTrans(const Double_t arr[16]);
  //ZTrans(const Float_t  arr[16]);
  ~ZTrans() {}

  // Add this explicit setter -- otherwise CINT can't do it.
  using HTrans<Double_t>::SetBaseVec;
  void SetBaseVec(Int_t b, const ZPoint& v)
  { SetBaseVec(b, v.x, v.y, v.z); }

  ZTrans operator*(const ZTrans& t);

  ClassDefNV(ZTrans, 1); // Column-major 4x4 matrix for homogeneous coordinates.
};

} // endnamespace gled

#endif
