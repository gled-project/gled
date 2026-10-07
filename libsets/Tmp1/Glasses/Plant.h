// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

/**************************************************************************/
// Plant - extension of Weed class. Has virtual functions to draw
// segments, leaf and flower as polygons.
/**************************************************************************/

#ifndef Tmp1_Plant_H
#define Tmp1_Plant_H

#include <Glasses/Weed.h>

namespace gled {

class Plant : public Weed
{
  MAC_RNR_FRIENDS(Plant);
  
private:
  void _init();
  
protected:
  
public:
  Plant(const Text_t* n="Plant", const Text_t* t=0);
  virtual ~Plant();
  
  Double_t mStemWidth;   // X{GST} 7 Value(-range=>[0, 1, 1, 1000])
  Double_t mLeafSize;    // X{GST} 7 Value(-range=>[0, 1, 1, 100])
  Double_t mFlowerSize;  // X{GST} 7 Value(-range=>[0, 1, 1, 100])
  
#include "Plant.h7"
  ClassDef(Plant, 1);
}; // endclass Plant

} // endnamespace gled

#endif
