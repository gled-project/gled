// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef Var1_Chopper_H
#define Var1_Chopper_H

#include <Glasses/Flyer.h>

namespace gled {

class Chopper : public Flyer
{
  MAC_RNR_FRIENDS(Chopper);

private:
  void _init();

protected:

public:
  Chopper(const Text_t* n="Chopper", const Text_t* t=0);
  virtual ~Chopper();

#include "Chopper.h7"
  ClassDef(Chopper, 1);
}; // endclass Chopper

} // endnamespace gled

#endif
