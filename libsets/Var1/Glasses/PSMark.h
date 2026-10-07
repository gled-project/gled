// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef Var1_PSMark_H
#define Var1_PSMark_H

#include <Glasses/ZNode.h>

namespace gled {

class ParaSurf;

class PSMark : public ZNode
{
  MAC_RNR_FRIENDS(PSMark);

private:
  void _init();

protected:
  ZLink<ParaSurf>   mParaSurf;  //  X{GS} L{aA}

  Float_t mF;    //  X{GE}  7 Value(-range=>[-1e5,1e5,1,1000], -join=>1)
  Float_t mG;    //  X{GE}  7 Value(-range=>[-1e5,1e5,1,1000])
  Float_t mH;    //  X{GE}  7 Value(-range=>[-1e5,1e5,1,1000], -join=>1)

  void retrans(ParaSurf* ps);

public:
  PSMark(const Text_t* n="PSMark", const Text_t* t=0);
  PSMark(ParaSurf* ps, const Text_t* n="PSMark", const Text_t* t=0);

  void SetF(Float_t f);
  void SetG(Float_t g);
  void SetH(Float_t h);
  void SetHRel(Float_t hr);

#include "PSMark.h7"
  ClassDef(PSMark, 1);
}; // endclass PSMark


} // endnamespace gled

#endif
