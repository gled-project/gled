// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef GledCore_WGlValuator_GL_RNR_H
#define GledCore_WGlValuator_GL_RNR_H

#include <Glasses/WGlValuator.h>
#include <Glasses/WGlFrameStyle.h>
#include <Glasses/ZRlFont.h>
#include <Rnr/GL/ZNode_GL_Rnr.h>

namespace gled {

class WGlValuator_GL_Rnr : public ZNode_GL_Rnr
{
private:
  void _init();

protected:
  WGlValuator*	mWGlValuator;

  RnrModStore	mFontRMS;
  RnrModStore	mFrameRMS;

  Bool_t	bBelowMouse;

  Int_t         mX, mY;
  Double_t      mButtFac;

  Double_t get_value();
  Bool_t   send_value(Double_t step_base);

public:
  WGlValuator_GL_Rnr(WGlValuator* idol) :
    ZNode_GL_Rnr(idol), mWGlValuator(idol),
    mFontRMS(ZRlFont::FID()), mFrameRMS(WGlFrameStyle::FID())
  { _init(); }

  virtual void Draw(RnrDriver* rd);

  virtual int  Handle(RnrDriver* rd, Fl_Event& ev);

}; // endclass WGlValuator_GL_Rnr

} // endnamespace gled

#endif
