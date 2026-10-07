// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef Tmp1_TabletStroke_GL_RNR_H
#define Tmp1_TabletStroke_GL_RNR_H

#include <Glasses/TabletStroke.h>
#include <Rnr/GL/ZNode_GL_Rnr.h>

#include <Glasses/TabletRnrMod.h>
#include <Rnr/GL/TabletRnrMod_GL_Rnr.h>

namespace gled {

class TabletStroke_GL_Rnr : public ZNode_GL_Rnr
{
private:
  void _init();

protected:
  TabletStroke*	mTabletStroke;

  RnrModStoreT<TabletRnrMod> mTabletRMS;

public:
  TabletStroke_GL_Rnr(TabletStroke* idol);
  virtual ~TabletStroke_GL_Rnr();

  //virtual void PreDraw(RnrDriver* rd);
  virtual void Draw(RnrDriver* rd);
  //virtual void PostDraw(RnrDriver* rd);

  virtual void Render(RnrDriver* rd);

}; // endclass TabletStroke_GL_Rnr

} // endnamespace gled

#endif
