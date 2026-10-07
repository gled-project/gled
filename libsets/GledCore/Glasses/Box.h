// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef GledCore_Box_H
#define GledCore_Box_H

#include <Glasses/ZNode.h>
#include <Stones/ZColor.h>

namespace gled {

class Box : public ZNode {
  MAC_RNR_FRIENDS(Box);

private:
  void _init();

protected:
  Float_t	mA;	// X{GS}  7 Value(-range=>[0,1000,1,100], -join=>1)
  Float_t	mB;	// X{GS}  7 Value(-range=>[0,1000,1,100], -join=>1)
  Float_t	mC;	// X{GS}  7 Value(-range=>[0,1000,1,100])
  ZColor	mColor;	// X{PGS} 7 ColorButt()
public:
  Box(const Text_t* n="Box", const Text_t* t=0) : ZNode(n,t) { _init(); }

  void SetABC(Float_t a, Float_t b, Float_t c); // X{E}

#include "Box.h7"
  ClassDef(Box, 1);
}; // endclass Box


} // endnamespace gled

#endif
