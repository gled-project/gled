// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef Var1_TringuObserverSpiritio_H
#define Var1_TringuObserverSpiritio_H

#include <Glasses/Spiritio.h>

#include <Stones/SVars.h>

namespace gled {

class TringuCam;

class ZNode;

class TringuObserverSpiritio : public Spiritio
{
  MAC_RNR_FRIENDS(TringuObserverSpiritio);

private:
  void _init();

protected:
  ZLink<TringuCam>  mTringuCam; // X{GS} L{}

  SDesireVarF mFwdBck;
  SDesireVarF mLftRgt;
  SDesireVarF mUpDown;

  SDesireVarF mSpinUp; // Spin about parent's up axis

  Int_t     mStampInterval;     //  X{GS} 7 Value(-range=>[0,1000])
  Int_t     mStampCount;        //!
  Double_t  mHeight;            //  Height above tringula. X{GS} 7 Value(-range=>[-1e5, 1e5, 1, 100])

public:
  TringuObserverSpiritio(const Text_t* n="TringuObserverSpiritio", const Text_t* t=0);
  virtual ~TringuObserverSpiritio();

  virtual void AdEnlightenment();

  // Spiritio
  virtual void Activate();
  virtual void Deactivate();

  // TimeMakerClient
  virtual void TimeTick(Double_t t, Double_t dt);

#include "TringuObserverSpiritio.h7"
  ClassDef(TringuObserverSpiritio, 1);
}; // endclass TringuObserverSpiritio

} // endnamespace gled

#endif
