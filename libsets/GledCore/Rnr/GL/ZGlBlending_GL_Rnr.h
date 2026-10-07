// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef GledCore_ZGlBlending_GL_RNR_H
#define GledCore_ZGlBlending_GL_RNR_H

#include <Glasses/ZGlBlending.h>
#include <Rnr/GL/ZRnrModBase_GL_Rnr.h>

namespace gled {

class ZGlBlending_GL_Rnr : public ZRnrModBase_GL_Rnr {
private:
  void _init();

protected:
  ZGlBlending*	mZGlBlending;

public:
  ZGlBlending_GL_Rnr(ZGlBlending* idol) : ZRnrModBase_GL_Rnr(idol), mZGlBlending(idol) {}

  virtual void PreDraw(RnrDriver* rd);
  virtual void Draw(RnrDriver* rd);
  virtual void PostDraw(RnrDriver* rd);

  void SetupGL(RnrDriver* rd);

}; // endclass ZGlBlending_GL_Rnr

} // endnamespace gled

#endif
