// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef Var1_TringuObserver_H
#define Var1_TringuObserver_H

#include <Glasses/ZNode.h>

namespace gled {

class TringuObserver : public ZNode
{
  MAC_RNR_FRIENDS(TringuObserver);

private:
  void _init();

protected:

public:
  TringuObserver(const Text_t* n="TringuObserver", const Text_t* t=0);
  virtual ~TringuObserver();

#include "TringuObserver.h7"
  ClassDef(TringuObserver, 1);
}; // endclass TringuObserver

} // endnamespace gled

#endif
