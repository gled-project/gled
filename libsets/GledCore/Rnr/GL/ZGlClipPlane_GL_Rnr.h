// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef GledCore_ZGlClipPlane_GL_RNR_H
#define GledCore_ZGlClipPlane_GL_RNR_H

#include <Glasses/ZGlClipPlane.h>
#include <Rnr/GL/ZRnrModBase_GL_Rnr.h>

namespace gled {

class ZGlClipPlane_GL_Rnr : public ZRnrModBase_GL_Rnr {
private:
  void _init();

protected:
  ZGlClipPlane*	mZGlClipPlane;
  Int_t         mClipId;

public:
  ZGlClipPlane_GL_Rnr(ZGlClipPlane* idol) :
    ZRnrModBase_GL_Rnr(idol), mZGlClipPlane(idol)
  { _init(); }

  virtual void PreDraw(RnrDriver* rd);
  virtual void Draw(RnrDriver* rd);
  virtual void PostDraw(RnrDriver* rd);

  virtual void CleanUp(RnrDriver* rd);

  virtual void TurnOn(RnrDriver* rd);
  virtual void TurnOff(RnrDriver* rd);

  void RnrSelf();

}; // endclass ZGlClipPlane_GL_Rnr

} // endnamespace gled

#endif
