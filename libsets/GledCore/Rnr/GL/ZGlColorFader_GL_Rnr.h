// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef GledCore_ZGlColorFader_GL_RNR_H
#define GledCore_ZGlColorFader_GL_RNR_H

#include <Glasses/ZGlColorFader.h>
#include <Rnr/GL/ZRnrModBase_GL_Rnr.h>

namespace gled {

class ZGlColorFader_GL_Rnr : public ZRnrModBase_GL_Rnr {
private:
  void _init();

protected:
  ZGlColorFader* mZGlColorFader;

public:
  ZGlColorFader_GL_Rnr(ZGlColorFader* idol) :
    ZRnrModBase_GL_Rnr(idol), mZGlColorFader(idol)
  { _init(); }

  virtual void PreDraw(RnrDriver* rd);
  virtual void Draw(RnrDriver* rd);
  virtual void PostDraw(RnrDriver* rd);
}; // endclass ZGlColorFader_GL_Rnr

} // endnamespace gled

#endif
