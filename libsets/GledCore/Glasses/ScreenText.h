// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef GledCore_ScreenText_H
#define GledCore_ScreenText_H

#include <Glasses/ZGlass.h>
#include <Stones/ZColor.h>

namespace gled {

class ScreenText : public ZGlass
{
  MAC_RNR_FRIENDS(ScreenText);

private:
  void _init();

protected:
  TString	mText;  // X{GRS} 7 Textor()
  ZColor	mFgCol; // X{GSP} 7 ColorButt(-join=>1)
  ZColor	mBgCol; // X{GSP} 7 ColorButt()
  Int_t		mX;     // X{GS}  7 Value(-range=>[-1000,1000,1], -join=>1)
  Int_t		mY;     // X{GS}  7 Value(-range=>[-300,300,1], -join=>1)
  Float_t	mZ;     // X{GS}  7 Value(-range=>[0,1,1,1000])

public:
  ScreenText(const Text_t* n="ScreenText", const Text_t* t=0) :
    ZGlass(n,t) { _init(); }

#include "ScreenText.h7" // Text display in window coordinates.
  ClassDef(ScreenText, 1);
}; // endclass ScreenText


} // endnamespace gled

#endif
