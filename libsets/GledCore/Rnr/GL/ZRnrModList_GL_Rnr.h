// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef GledCore_ZRnrModList_GL_RNR_H
#define GledCore_ZRnrModList_GL_RNR_H

#include <Glasses/ZRnrModList.h>
#include <Rnr/GL/ZGlass_GL_Rnr.h>

namespace gled {

class ZRnrModList_GL_Rnr : public ZGlass_GL_Rnr
{
private:
  void _init();

protected:
  ZRnrModList*	mZRnrModList;

public:
  ZRnrModList_GL_Rnr(ZRnrModList* idol) :
    ZGlass_GL_Rnr(idol), mZRnrModList(idol)
  { _init(); }

  virtual void PreDraw(RnrDriver* rd);
  virtual void PostDraw(RnrDriver* rd);

}; // endclass ZRnrModList_GL_Rnr

} // endnamespace gled

#endif
