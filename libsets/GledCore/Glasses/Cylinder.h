// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef GledCore_Cylinder_H
#define GledCore_Cylinder_H

#include <Glasses/ZNode.h>
#include <Stones/ZColor.h>

namespace gled {

class Cylinder : public ZNode {
  MAC_RNR_FRIENDS(Cylinder);

public:
  enum Orientation_e { O_X, O_Y, O_Z };
private:
  void _init();

protected:
  Orientation_e mOrientation;   // X{GST}  7 PhonyEnum(-join=>1)
  Float_t       mPhiOffset;     // X{GST}  7 Value(-range=>[-0.5,0.5, 1,1000])

  Float_t       mHeight;        // X{GST}  7 Value(-range=>[0,1000,1,1000], -join=>1)
  Bool_t        bRnrDisks;      // X{gST}  7 Bool()
  Float_t	mRInBase;	// X{GST}  7 Value(-range=>[0,1000,1,1000], -join=>1)
  Float_t	mRInTop;	// X{GST}  7 Value(-range=>[0,1000,1,1000])
  Float_t	mROutBase;	// X{GST}  7 Value(-range=>[0,1000,1,1000], -join=>1)
  Float_t	mROutTop;	// X{GST}  7 Value(-range=>[0,1000,1,1000])
  ZColor	mColor;		// X{PGST} 7 ColorButt()
  Int_t		mLodH;		// X{GST}  7 Value(-range=>[1,100,1,1], -join=>1)
  Int_t		mLodPhi;	// X{GST}  7 Value(-range=>[1,100,1,1])

public:
  Cylinder(const Text_t* n="Cylinder", const Text_t* t=0) :
    ZNode(n,t) { _init(); }

#include "Cylinder.h7"
  ClassDef(Cylinder, 1); // Simple cylinder, possibly hollow.
}; // endclass Cylinder


} // endnamespace gled

#endif
