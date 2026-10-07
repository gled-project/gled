// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef GledCore_ScreenText_GL_RNR_H
#define GledCore_ScreenText_GL_RNR_H

#include <Glasses/ScreenText.h>
#include <Rnr/GL/ZGlass_GL_Rnr.h>

namespace gled {

class ScreenText_GL_Rnr : public ZGlass_GL_Rnr {
private:
  void _init();

protected:
  ScreenText*	mScreenText;

public:
  ScreenText_GL_Rnr(ScreenText* idol) :
    ZGlass_GL_Rnr(idol), mScreenText(idol)
  { _init(); }

  //virtual void PreDraw(RnrDriver* rd);
  virtual void Draw(RnrDriver* rd);
  //virtual void PostDraw(RnrDriver* rd);

}; // endclass ScreenText_GL_Rnr

} // endnamespace gled

#endif
