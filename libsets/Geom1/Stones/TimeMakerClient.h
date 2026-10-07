// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef Geom1_TimeMakerClient_H
#define Geom1_TimeMakerClient_H

#include <Rtypes.h>

namespace gled {

class TimeMakerClient
{
public:
  virtual ~TimeMakerClient() {}

  virtual void TimeTick(Double_t time, Double_t delta) = 0;

  ClassDef(TimeMakerClient, 0);
}; // endclass TimeMakerClient

} // endnamespace gled

#endif
