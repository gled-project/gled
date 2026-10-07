// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef Tmp1_TabletRnrMod_GL_RNR_H
#define Tmp1_TabletRnrMod_GL_RNR_H

#include <Glasses/TabletRnrMod.h>
#include <Rnr/GL/ZRnrModBase_GL_Rnr.h>

namespace gled {

class TabletRnrMod_GL_Rnr : public ZRnrModBase_GL_Rnr
{
private:
  void _init();

protected:
  TabletRnrMod*	mTabletRnrMod;

public:
  TabletRnrMod_GL_Rnr(TabletRnrMod* idol);
  virtual ~TabletRnrMod_GL_Rnr();

  virtual void PreDraw(RnrDriver* rd);
  virtual void Draw(RnrDriver* rd);
  virtual void PostDraw(RnrDriver* rd);

  // virtual void Render(RnrDriver* rd);

}; // endclass TabletRnrMod_GL_Rnr

} // endnamespace gled

#endif
