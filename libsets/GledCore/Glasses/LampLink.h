// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef GledCore_LampLink_H
#define GledCore_LampLink_H

#include <Glasses/ZGlass.h>
#include <Glasses/Lamp.h>

namespace gled {

class LampLink : public ZGlass {
  // 7777 RnrCtrl(RnrBits(0,4,0,0))
  MAC_RNR_FRIENDS(LampLink);

private:
  void _init();

protected:
  ZLink<Lamp>		mLamp;	  // X{gS} L{} RnrBits zero
  Bool_t	bTurnOn;  // X{gS} 7 Bool(-join=>1)
  Bool_t	bTurnOff; // X{gS} 7 Bool()

public:
  LampLink(const Text_t* n="LampLink", const Text_t* t=0) : ZGlass(n,t) { _init(); }

#include "LampLink.h7"
  ClassDef(LampLink, 1);
}; // endclass LampLink

} // endnamespace gled

#endif
