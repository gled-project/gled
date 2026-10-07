// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef GledCore_LampLink_GL_RNR_H
#define GledCore_LampLink_GL_RNR_H

#include <Glasses/LampLink.h>
#include <Rnr/GL/ZGlass_GL_Rnr.h>

namespace gled {

class LampLink_GL_Rnr : public ZGlass_GL_Rnr {
private:
  void _init();

protected:
  Bool_t        bWarn;
  LampLink*	mLampLink;

public:
  LampLink_GL_Rnr(LampLink* idol) : ZGlass_GL_Rnr(idol), mLampLink(idol) { _init(); }

  virtual void AbsorbRay(Ray& ray);

  virtual void Draw(RnrDriver* rd);

}; // endclass LampLink_GL_Rnr

} // endnamespace gled

#endif
