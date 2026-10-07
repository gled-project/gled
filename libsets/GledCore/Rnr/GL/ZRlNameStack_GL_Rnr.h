// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef GledCore_ZRlNameStack_GL_RNR_H
#define GledCore_ZRlNameStack_GL_RNR_H

#include <Glasses/ZRlNameStack.h>
#include <Rnr/GL/ZRnrModBase_GL_Rnr.h>

namespace gled {

class ZRlNameStack_GL_Rnr : public ZRnrModBase_GL_Rnr {
private:
  void _init();

protected:
  bool          bExState;
  ZRlNameStack*	mZRlNameStack;

public:
  ZRlNameStack_GL_Rnr(ZRlNameStack* idol) :
    ZRnrModBase_GL_Rnr(idol), mZRlNameStack(idol)
  { _init(); }

  virtual void PreDraw(RnrDriver* rd);
  virtual void Draw(RnrDriver* rd);
  virtual void PostDraw(RnrDriver* rd);

}; // endclass ZRlNameStack_GL_Rnr

} // endnamespace gled

#endif
