// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef Var1_Explosion_H
#define Var1_Explosion_H

#include <Glasses/ZGlass.h>
#include <Stones/TimeMakerClient.h>

namespace gled {

class Tringula;

class Explosion : public ZGlass,
                  public TimeMakerClient
{
  MAC_RNR_FRIENDS(Explosion);

private:
  void _init();

protected:
  Tringula       *mTringula;        //! X{g}

  Float_t         mExplodeTime;     // X{GS}
  Float_t         mExplodeDuration; // X{GS}

public:
  Explosion(const Text_t* n="Explosion", const Text_t* t=0);
  virtual ~Explosion();

  virtual void SetTringula(Tringula* tring);

  virtual void TimeTick(Double_t t, Double_t dt);

#include "Explosion.h7"
  ClassDef(Explosion, 1);
}; // endclass Explosion

} // endnamespace gled

#endif
