// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef GledCore_WGlFrameStyle_H
#define GledCore_WGlFrameStyle_H

#include <Glasses/ZRnrModBase.h>
#include <Stones/ZColor.h>

namespace gled {

class WGlFrameStyle : public ZRnrModBase
{
  // 7777 RnrCtrl(RnrBits(0,4,0,0))
  MAC_RNR_FRIENDS(WGlFrameStyle);

public:
  enum HAlign_e { HA_Left,   HA_Center, HA_Right };
  enum VAlign_e { VA_Bottom, VA_Center, VA_Top   };

private:
  void _init();

protected:
  HAlign_e          mHAlign;       // X{GS} 7 PhonyEnum(-join=>1)
  VAlign_e          mVAlign;       // X{GS} 7 PhonyEnum()
  Float_t           mDefDx;        // X{GS} 7 Value(-range=>[0,1000,1,1000], -join=>1)
  Float_t           mTextFadeW;    // X{GS} 7 Value(-range=>[0,1,1,1000])
  Float_t           mTextYSize;    // X{GS} 7 Value(-range=>[0,1000,1,1000], -join=>1)
  Float_t           mFrameW;       // X{GS} 7 Value(-range=>[0,128,1,100])

  Bool_t            bDrawTile;     // X{GS} 7 Bool(-join=>1)
  Bool_t            bMinTile;      // X{GS} 7 Bool(-join=>1)
  Bool_t            bDrawFrame;    // X{GS} 7 Bool()
  Float_t           mXBorder;      // X{GS} 7 Value(-range=>[-100,100,1,1000], -join=>1)
  Float_t           mYBorder;      // X{GS} 7 Value(-range=>[-100,100,1,1000])

  ZColor            mTextColor;    // X{GSP} 7 ColorButt(-join=>1)
  ZColor            mFrameColor;   // X{GSP} 7 ColorButt()
  ZColor            mTileColor;    // X{GSP} 7 ColorButt(-join=>1)
  ZColor            mBelowMColor;  // X{GSP} 7 ColorButt()

public:
  WGlFrameStyle(const Text_t* n="WGlFrameStyle", const Text_t* t=0) :
    ZRnrModBase(n,t) { _init(); }

  void StandardPersp();  //  X{E} 7 MButt(-join=>1)
  void StandardFixed();  //  X{E} 7 MButt(-join=>1)
  void StandardPixel();  //  X{E} 7 MButt()

#include "WGlFrameStyle.h7"
  ClassDef(WGlFrameStyle, 1);
}; // endclass WGlFrameStyle


} // endnamespace gled

#endif
