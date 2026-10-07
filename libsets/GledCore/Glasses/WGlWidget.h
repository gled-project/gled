// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef GledCore_WGlWidget_H
#define GledCore_WGlWidget_H

#include <Glasses/ZNode.h>

namespace gled {

class WGlWidget : public ZNode
{
  MAC_RNR_FRIENDS(WGlWidget);

private:
  void _init();

protected:
  Float_t   mDx; // X{GST} 7 Value(-range=>[0,1000,1,1000], -join=>1)
  Float_t   mDy; // X{GST} 7 Value(-range=>[0,1000,1,1000])

  ZLink<ZGlass>   mCbackAlpha;      //  X{GS} L{} Ray{CbackReset}

public:
  WGlWidget(const Text_t* n="WGlWidget", const Text_t* t=0) :
    ZNode(n,t) { _init(); }

  virtual void EmitCbackResetRay() {}

  virtual void SetCbackBeta(ZGlass* lens) {}

  void SetDaughterCbackAlpha(ZGlass* lens, Int_t recurse_lvl=0); // X{E} C{1} 7 MCWButt()
  void SetDaughterCbackStuff(ZGlass* lens, Int_t recurse_lvl=0); // X{E} C{1} 7 MCWButt()

#include "WGlWidget.h7"
  ClassDef(WGlWidget, 1);
}; // endclass WGlWidget


} // endnamespace gled

#endif
