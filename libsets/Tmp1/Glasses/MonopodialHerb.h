// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef Tmp1_MonopodialHerb_H
#define Tmp1_MonopodialHerb_H

#include <Glasses/GrowingPlant.h>

namespace gled {

class MonopodialHerb : public GrowingPlant
{
  MAC_RNR_FRIENDS(MonopodialHerb);

private:
  void _init();

protected:
  virtual void SegmentStepTime(Segments_i ref,  Segments_t& in, Segments_t& out);  
  
public:
  MonopodialHerb(const Text_t* n="MonopodialHerb", const Text_t* t=0);
  virtual ~MonopodialHerb();

#include "MonopodialHerb.h7"
  ClassDef(MonopodialHerb, 1);
}; // endclass MonopodialHerb

} // endnamespace gled

#endif
