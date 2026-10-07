// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

//__________________________________________________________________________
// PerfMeterTarget
//
//

#include "PerfMeterTarget.h"

using namespace gled;

#include "PerfMeterTarget.c7"

/**************************************************************************/

void PerfMeterTarget::_init()
{
  mCount = 0;
}

/**************************************************************************/

void PerfMeterTarget::AssignVector(TVector& vec)
{
  mVector.ResizeTo(vec);
  mVector = vec;
}

/**************************************************************************/

void PerfMeterTarget::NullMethod()
{
}

void PerfMeterTarget::IncCount()
{
  WriteLock();
  ++mCount;
  Stamp(FID());
  WriteUnlock();
}

/**************************************************************************/
