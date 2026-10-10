// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef GledGTS_GTSIsoMakerFunctor_H
#define GledGTS_GTSIsoMakerFunctor_H

#include "Stones/HTrans.h"

namespace gled {

class GTSIsoMaker;

class GTSIsoMakerFunctor
{
public:
  virtual ~GTSIsoMakerFunctor() {}

  virtual void     GTSIsoBegin(GTSIsoMaker* maker, Double_t iso_value) {}
  // A potential: the inside is where it exceeds the iso value.
  virtual Double_t GTSIsoFunc(Double_t x, Double_t y, Double_t z) = 0;
  virtual Double_t GTSIsoGradient(Double_t x, Double_t y, Double_t z, HPointD& g) = 0;
  virtual void     GTSIsoEnd() {}

#include "GTSIsoMakerFunctor.h7"
  ClassDef(GTSIsoMakerFunctor, 0);
}; // endclass GTSIsoMakerFunctor

} // endnamespace gled

#endif
