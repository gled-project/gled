// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef Var1_LaserTraceExplosion_H
#define Var1_LaserTraceExplosion_H

#include <Glasses/Explosion.h>
#include <Stones/HTrans.h>

namespace gled {

class LaserTraceExplosion : public Explosion
{
  MAC_RNR_FRIENDS(LaserTraceExplosion);

protected:
  HPointF      mA;         // X{r}
  HPointF      mB;         // X{r}
  Float_t      mEndRadius; // X{gs}

public:
  LaserTraceExplosion(const Text_t* n="LaserTraceExplosion", const Text_t* t=0);
  virtual ~LaserTraceExplosion();

#include "LaserTraceExplosion.h7"
  ClassDef(LaserTraceExplosion, 1);
}; // endclass LaserTraceExplosion

} // endnamespace gled

#endif
