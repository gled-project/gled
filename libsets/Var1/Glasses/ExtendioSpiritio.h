// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef Var1_ExtendioSpiritio_H
#define Var1_ExtendioSpiritio_H

#include <Glasses/Spiritio.h>

namespace gled {

class Extendio;

class ExtendioSpiritio : public Spiritio
{
  MAC_RNR_FRIENDS(ExtendioSpiritio);

private:
  void _init();

protected:
  ZLink<Extendio> mExtendio;     // X{GS} L{f}
  FID_t           mExtendio_fid; //!

public:
  ExtendioSpiritio(const Text_t* n="ExtendioSpiritio", const Text_t* t=0);
  virtual ~ExtendioSpiritio();

  // Should keep abstract? If yes, tag in catalog.patch and remove the line.
  virtual void TimeTick(Double_t t, Double_t dt) {}

#include "ExtendioSpiritio.h7"
  ClassDef(ExtendioSpiritio, 1);
}; // endclass ExtendioSpiritio

} // endnamespace gled

#endif
