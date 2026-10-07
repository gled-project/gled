// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef Gled_Sphere_H
#define Gled_Sphere_H

#include <Glasses/ZNode.h>
#include <Stones/ZColor.h>

namespace gled {

class Sphere : public ZNode
{
  MAC_RNR_FRIENDS(Sphere);

protected:
  Float_t       mRadius;        // X{GS}   7 Value(-range=>[0,1000,1,1000])
  Int_t	        mLOD;           // X{GS}   7 Value(-range=>[1,100,1,1], -join=>1)
  ZColor        mColor;         // X{PRGS} 7 ColorButt()

  void _init();

public:
  Sphere(const Text_t* n="Sphere", const Text_t* t=0) :
    ZNode(n, t), mRadius(1) { _init(); }
  Sphere(Float_t r, const Text_t* n="Sphere", Text_t* t=0) :
    ZNode(n, t), mRadius(r) { _init();}

#include "Sphere.h7"
  ClassDef(Sphere, 1);
}; // endclass Sphere


} // endnamespace gled

#endif
