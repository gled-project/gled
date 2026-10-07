// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef Var1_Statico_H
#define Var1_Statico_H

#include <Glasses/Extendio.h>

namespace gled {

class Statico : public Extendio
{
  friend class Tringula;
  MAC_RNR_FRIENDS(Statico);

private:
  void _init();

protected:
  Int_t  mNDynoColls; // X{GS} 7 ValOut()

public:
  Statico(const Text_t* n="Statico", const Text_t* t=0) :
    Extendio(n,t) { _init(); }

  virtual void TimeTick(Double_t t, Double_t dt) {}

#include "Statico.h7"
  ClassDef(Statico, 1);
}; // endclass Statico

} // endnamespace gled

#endif
