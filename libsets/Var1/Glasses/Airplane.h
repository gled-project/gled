// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef Var1_Airplane_H
#define Var1_Airplane_H

#include <Glasses/Flyer.h>

namespace gled {

class Airplane : public Flyer
{
  MAC_RNR_FRIENDS(Airplane);

private:
  void _init();

protected:

public:
  Airplane(const Text_t* n="Airplane", const Text_t* t=0);
  virtual ~Airplane();

#include "Airplane.h7"
  ClassDef(Airplane, 1);
}; // endclass Airplane

} // endnamespace gled

#endif
