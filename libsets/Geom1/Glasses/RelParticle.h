// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef Geom1_RelParticle_H
#define Geom1_RelParticle_H

#include <Glasses/ZGlass.h>
#include <TLorentzVector.h>

namespace gled {

class RelParticle : public ZGlass {
  MAC_RNR_FRIENDS(RelParticle);

private:
  void _init();

protected:
  TLorentzVector	mX;	// X{GSR} 7 LorentzVector()
  TLorentzVector	mP;	// X{GSR} 7 LorentzVector()

public:
  RelParticle(const Text_t* n="RelParticle", const Text_t* t=0) : ZGlass(n,t) { _init(); }


#include "RelParticle.h7"
  ClassDef(RelParticle, 1);
}; // endclass RelParticle


} // endnamespace gled

#endif
