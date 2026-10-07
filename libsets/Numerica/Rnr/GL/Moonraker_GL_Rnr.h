// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef Numerica_Moonraker_GL_RNR_H
#define Numerica_Moonraker_GL_RNR_H

#include <Glasses/Moonraker.h>
#include <Rnr/GL/ZNode_GL_Rnr.h>

#include <GL/glew.h>

namespace gled {

class Moonraker_GL_Rnr : public ZNode_GL_Rnr
{
private:
  void _init();

protected:
  Moonraker*	 mMoonraker;

  GLUquadricObj* mQuadric;

public:
  Moonraker_GL_Rnr(Moonraker* idol) : ZNode_GL_Rnr(idol), mMoonraker(idol)
  { _init(); }
  virtual ~Moonraker_GL_Rnr();

  virtual void Draw(RnrDriver* rd);

}; // endclass Moonraker_GL_Rnr

} // endnamespace gled

#endif
