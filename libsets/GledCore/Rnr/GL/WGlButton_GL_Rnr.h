// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef GledCore_WGlButton_GL_RNR_H
#define GledCore_WGlButton_GL_RNR_H

#include <Glasses/WGlButton.h>
#include <Glasses/WGlFrameStyle.h>
#include <Glasses/ZRlFont.h>
#include <Rnr/GL/ZNode_GL_Rnr.h>

namespace gled {

class WGlButton_GL_Rnr : public ZNode_GL_Rnr
{
private:
  void _init();

protected:
  WGlButton*	mWGlButton;
  RnrModStore	mFontRMS;
  RnrModStore	mFrameRMS;

  Bool_t	bBelowMouse;

public:
  WGlButton_GL_Rnr(WGlButton* idol) :
    ZNode_GL_Rnr(idol), mWGlButton(idol),
    mFontRMS(ZRlFont::FID()), mFrameRMS(WGlFrameStyle::FID())
  { _init(); }

  virtual void Draw(RnrDriver* rd);

  virtual int  Handle(RnrDriver* rd, Fl_Event& ev);

}; // endclass WGlButton_GL_Rnr

} // endnamespace gled

#endif
