// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef Geom1_TimeScreenText_H
#define Geom1_TimeScreenText_H

#include <Glasses/ScreenText.h>

#include <Stones/TimeMakerClient.h>

namespace gled {

class TimeScreenText : public ScreenText, public TimeMakerClient
{
  MAC_RNR_FRIENDS(TimeScreenText);

private:
  void _init();

protected:
  TString       mFormat; // X{GS} 7 Textor()

public:
  TimeScreenText(const Text_t* n="TimeScreenText", const Text_t* t=0) :
    ScreenText(n,t) { _init(); }

  // TimeMakerClient
  virtual void TimeTick(Double_t t, Double_t dt);

#include "TimeScreenText.h7"
  ClassDef(TimeScreenText, 1);
}; // endclass TimeScreenText


} // endnamespace gled

#endif
