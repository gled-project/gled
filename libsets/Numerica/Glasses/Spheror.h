// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef Numerica_Spheror_H
#define Numerica_Spheror_H

#include <Glasses/ZNode.h>
#include <Glasses/WarmAmoeba.h>

#include <TVectorF.h>
#include <Stones/ZColor.h>

namespace gled {

class Spheror : public ZNode, public WarmAmoebaMaster {
  MAC_RNR_FRIENDS(Spheror);

private:
  void _init();

protected:
  Int_t	mNVert;		// X{gS} 7 Value(-range=>[2,100,1,1], -join=>1)
private:
  Int_t	mNV;		// X{g}  7 ValOut(-range=>[2,100,1,1])

protected:
  ZLink<WarmAmoeba>	mAmoeba;	// X{gS} L{}

  TVectorF	mState;

  Bool_t	bBeautyP;	// X{gS}  7 Bool(-join=>1)
  ZColor	mColor;		// X{PGS} 7 ColorButt()
  Float_t	mSize;		// X{gS}  7 Value(-range=>[0,100,1,100], -join=>1)
  Float_t	mScale;		// X{gS}  7 Value(-range=>[0,100,1,10])

  TVectorF*	GimmeXYZ(const TVectorF& x);

public:
  Spheror(const Text_t* n="Spheror", const Text_t* t=0) : ZNode(n,t) {_init();}

  // virtuals
  virtual Float_t	Foon(const TVectorF& x);
  virtual TVectorF*	InitialState(TRandom& rnd);
  virtual TVectorF*	InitialPerturbations(TRandom& rnd);
  virtual void		SetState(const TVectorF& x); // X{E}

  void Install();	// X{E} C{0} 7 MButt(-join=>1)
  void SelfInit();	// X{E} C{0} 7 MButt()

#include "Spheror.h7"
  ClassDef(Spheror, 1);
}; // endclass Spheror


} // endnamespace gled

#endif
