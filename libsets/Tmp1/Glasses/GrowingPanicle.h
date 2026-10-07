// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef Tmp1_GrowingPanicle_H
#define Tmp1_GrowingPanicle_H

#include <Glasses/GrowingPlant.h>

namespace gled {

class GrowingPanicle : public GrowingPlant
{
  MAC_RNR_FRIENDS(GrowingPanicle);

private:
  void _init();

protected:
  virtual void SegmentStepTime(Segments_i ref, Segments_t& in, Segments_t& out);  
  
  float mLateralAngle;          // X{GST} 7 Value(-range=>[0, 90, 1])
  
  ZColor		mSColor;  // X{PGST} 7 ColorButt(-join=>1)
  float     mSSize;  // X{GST} 7 Value(-range=>[0, 1, 1, 100])
  
  ZColor		mTColor;  // X{PGST} 7 ColorButt(-join=>1)
  float     mTSize;  // X{GST} 7 Value(-range=>[0, 1, 1, 100])
  
  ZColor		mUColor;  // X{PGST} 7 ColorButt(-join=>1)
  float     mUSize;  // X{GST} 7 Value(-range=>[0, 1, 1, 100])
  
public:
  GrowingPanicle(const Text_t* n="GrowingPanicle", const Text_t* t=0);
  virtual ~GrowingPanicle();

#include "GrowingPanicle.h7"
  ClassDef(GrowingPanicle, 1);
}; // endclass GrowingPanicle

} // endnamespace gled

#endif
