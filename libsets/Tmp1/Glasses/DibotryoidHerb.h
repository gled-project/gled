// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef Tmp1_DibotryoidHerb_H
#define Tmp1_DibotryoidHerb_H

#include <Glasses/GrowingPlant.h>

namespace gled {

class DibotryoidHerb : public GrowingPlant
{
  MAC_RNR_FRIENDS(DibotryoidHerb);

private:
  void _init();

protected:
  virtual void SegmentStepTime(Segments_i ref, Segments_t& in, Segments_t& out);  
  
  int mSendSignalDelay;        // X{GST} 7 Value(-range=>[1, 20, 1])
  int mCreateBranchDelay;      // X{GST} 7 Value(-range=>[1, 10, 1])
  
  int mPlastocronMainAxis;     // X{GST} 7 Value(-range=>[1, 10, 1], -join=>1)
  int mPlastocronLateralAxis;  // X{GST} 7 Value(-range=>[1, 10, 1])
  
  int mSignalDelayMainAxis;    // X{GST} 7 Value(-range=>[1, 10, 1], -join=>1)
  int mSignalDelayLateralAxis; // X{GST} 7 Value(-range=>[1, 10, 1])
 
  float mLateralAngle;          // X{GST} 7 Value(-range=>[0, 90, 1])
  
  ZColor		mAColor;  // X{PGST} 7 ColorButt()
  ZColor		mBColor;  // X{PGST} 7 ColorButt()
  ZColor		mLColor;  // X{PGST} 7 ColorButt()

  
public:
  DibotryoidHerb(const Text_t* n="DibotryoidHerb", const Text_t* t=0);
  virtual ~DibotryoidHerb();
  virtual void  DumpList(Segments_t& slist) ;
  
#include "DibotryoidHerb.h7"
  ClassDef(DibotryoidHerb, 1);
}; // endclass DibotryoidHerb

} // endnamespace gled

#endif
