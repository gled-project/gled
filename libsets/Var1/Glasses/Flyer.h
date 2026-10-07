// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef Var1_Flyer_H
#define Var1_Flyer_H

#include <Glasses/Dynamico.h>

namespace gled {

class Flyer : public Dynamico
{
  MAC_RNR_FRIENDS(Flyer);

private:
  void _init();

protected:
  Float_t  mHeight;       // X{GS} 7 ValOut()

  Float_t  mGravHChange;
  Bool_t   bGravFixUpDir; // X{GS} 7 Bool()

  Float_t  mTerrainSafety;      //! Safe distance from the terrain.
  Float_t  mTerrainProbeRadius; //! Radius of the last terrain probe.

public:
  Flyer(const Text_t* n="Flyer", const Text_t* t=0);
  virtual ~Flyer();

  virtual void TimeTick(Double_t t, Double_t dt);

#include "Flyer.h7"
  ClassDef(Flyer, 1);
}; // endclass Flyer

} // endnamespace gled

#endif
