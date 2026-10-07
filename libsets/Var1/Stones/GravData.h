// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef Var1_GravData_H
#define Var1_GravData_H

#include <Gled/GledTypes.h>

#include <Opcode/Opcode.h>

#include <TObject.h>

namespace gled {

class GravData : public TObject
{
public:
  typedef std::list<std::pair<Float_t, GravData*> > lGravFraction_t;
  typedef lGravFraction_t::iterator                 lGravFraction_i;

protected:
  void clear();
  void add(Float_t f, GravData& gd);

public:
  Float_t fPos[3];  // Position where this data was calculated.
  Float_t fDir[3];  // Direction of gravity.
  Float_t fMag;
  Float_t fLDer;    // Derivative in longitudinal direction (direction of gravity).
  Float_t fTDer;    // Derivative in tangential plane.
  Float_t fH;       // Height as defined by the surface.
  Float_t fDown[3]; // Parametric down direction.

  // Storage for users of the struct, like Dynamico's.
  Float_t fSafeTime;     //!
  Float_t fSafeDistance; //!

  GravData() : fSafeTime(0), fSafeDistance(0) {}
  virtual ~GravData() {}

  Opcode::Point& Pos()  { return * (Opcode::Point*) fPos;  }
  Opcode::Point& Dir()  { return * (Opcode::Point*) fDir;  }
  Opcode::Point& Down() { return * (Opcode::Point*) fDown; }

  Bool_t DecaySafeties(Float_t dt, Float_t dl)
  { fSafeTime -= dt; fSafeDistance -= dl; return fSafeTime < 0 || fSafeDistance < 0; }

  void Combine(lGravFraction_t& fractions);

  virtual void Print(Option_t *option="") const;

#include "GravData.h7"
  ClassDef(GravData, 1);
}; // endclass GravData

} // endnamespace gled

#endif
