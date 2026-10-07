// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef Var1_CosmicBall_H
#define Var1_CosmicBall_H

#include <Glasses/Sphere.h>

namespace gled {

class CosmicBall : public Sphere
{
  MAC_RNR_FRIENDS(CosmicBall);
  friend class SolarSystem;

private:
  void _init();

protected:
  Double_t      mM; // X{GS}   7 Value()
  HPointD       mV; // X{GSRr} 7 HPointD()

  // Hack for orbit switching.
  Double_t      mDesiredR; // X{GS} 7 Value()

  std::vector<HPointF> mHistory;       //!
  Int_t           mHistorySize;   //!
  Int_t           mHistoryFirst;  //!
  Int_t           mHistoryStored; //!
  GMutex          mHistoryMoo;    //!

public:
  CosmicBall(const Text_t* n="CosmicBall", const Text_t* t=0) :
    Sphere(n,t) { _init(); }

  void StorePos();
  void ClearHistory();
  void ResizeHistory(Int_t size);

#include "CosmicBall.h7"
  ClassDef(CosmicBall, 1);
}; // endclass CosmicBall


} // endnamespace gled

#endif
