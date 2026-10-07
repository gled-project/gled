// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef Gled_GCondition_H
#define Gled_GCondition_H

#include <Gled/GledTypes.h>
#include <Gled/GMutex.h>
#include <Gled/GTime.h>
#include <pthread.h>

namespace gled {

class GCondition : public GMutex
{
private:
  pthread_cond_t	mCond;
public:
  GCondition(Init_e e=fast);
  ~GCondition();

  Int_t	Wait();
  Int_t TimedWait(GTime time);
  Int_t TimedWaitUntil(GTime time);
  Int_t	Signal();
  Int_t Broadcast();
  Int_t	LockSignal();
  Int_t LockBroadcast();

#include "GCondition.h7"
  ClassDefNV(GCondition,0);
}; // endclass GCondition

} // endnamespace gled

#endif
