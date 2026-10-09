// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef GledCore_ZRlFont_GL_RNR_H
#define GledCore_ZRlFont_GL_RNR_H

#include <Glasses/ZRlFont.h>
#include <Rnr/GL/ZRnrModBase_GL_Rnr.h>

namespace gled {

class FTFont;

class ZRlFont_GL_Rnr : public ZRnrModBase_GL_Rnr
{
private:
  void _init();

protected:
  ZRlFont	*mZRlFont;
  FTFont	*mFont;    // X{g}

public:
  ZRlFont_GL_Rnr(ZRlFont* idol) :
    ZRnrModBase_GL_Rnr(idol), mZRlFont(idol) { _init(); }
  virtual ~ZRlFont_GL_Rnr();

  virtual void AbsorbRay(Ray& ray);

  virtual void PreDraw(RnrDriver* rd);
  virtual void Draw(RnrDriver* rd);
  virtual void PostDraw(RnrDriver* rd);

  bool LoadFont();

#include "ZRlFont_GL_Rnr.h7"
}; // endclass ZRlFont_GL_Rnr

} // endnamespace gled

#endif
