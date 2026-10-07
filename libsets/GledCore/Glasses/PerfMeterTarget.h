// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef GledCore_PerfMeterTarget_H
#define GledCore_PerfMeterTarget_H

#include <Glasses/ZList.h>

#include <TVector.h>

namespace gled {

class PerfMeterTarget : public ZList
{
  MAC_RNR_FRIENDS(PerfMeterTarget);

private:
  void _init();

protected:
  UInt_t	mCount;		// X{GS} 7 Value()

  TVector	mVector;	// X{GRSQ}

public:
  PerfMeterTarget(const Text_t* n="PerfMeterTarget", const Text_t* t=0) : ZList(n,t) { _init(); }

  void AssignVector(TVector& vec); // X{E}

  void NullMethod();		// X{E}
  void IncCount();		// X{E}

#include "PerfMeterTarget.h7"
  ClassDef(PerfMeterTarget, 1);
}; // endclass PerfMeterTarget


} // endnamespace gled

#endif
