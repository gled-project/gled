// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef Tmp1_TabletReader_GL_RNR_H
#define Tmp1_TabletReader_GL_RNR_H

#include <Glasses/TabletReader.h>
#include <Rnr/GL/ZNode_GL_Rnr.h>

#include <Glasses/TabletRnrMod.h>
#include <Rnr/GL/TabletRnrMod_GL_Rnr.h>

namespace gled {

class TabletReader_GL_Rnr : public ZNode_GL_Rnr
{
private:
  void _init();

protected:
  TabletReader*	mTabletReader;

  RnrModStoreT<TabletRnrMod> mTabletRMS;

public:
  TabletReader_GL_Rnr(TabletReader* idol);
  virtual ~TabletReader_GL_Rnr();

  //virtual void PreDraw(RnrDriver* rd);
  virtual void Draw(RnrDriver* rd);
  //virtual void PostDraw(RnrDriver* rd);

  virtual void Render(RnrDriver* rd);

}; // endclass TabletReader_GL_Rnr

} // endnamespace gled

#endif
