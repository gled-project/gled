// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef Var1_ExtendioExplosion_H
#define Var1_ExtendioExplosion_H

#include <Glasses/Explosion.h>

namespace gled {

class Extendio;

class ExtendioExplosion : public Explosion
{
  MAC_RNR_FRIENDS(ExtendioExplosion);

protected:
  Extendio    *mExtendio; //! X{g}

public:
  ExtendioExplosion(const Text_t* n="ExtendioExplosion", const Text_t* t=0);
  virtual ~ExtendioExplosion();

  virtual void SetExtendio(Extendio* ext);

  virtual void TimeTick(Double_t t, Double_t dt);

#include "ExtendioExplosion.h7"
  ClassDef(ExtendioExplosion, 1);
}; // endclass ExtendioExplosion

} // endnamespace gled

#endif
