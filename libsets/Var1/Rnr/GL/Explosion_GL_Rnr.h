// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef Var1_Explosion_GL_RNR_H
#define Var1_Explosion_GL_RNR_H

#include <Glasses/Explosion.h>
#include <Rnr/GL/ZGlass_GL_Rnr.h>

namespace gled {

class Explosion_GL_Rnr : public ZGlass_GL_Rnr
{
private:
  void _init();

protected:
  Explosion*        mExplosion;

public:
  Explosion_GL_Rnr(Explosion* idol);
  virtual ~Explosion_GL_Rnr();

  // virtual void PreDraw(RnrDriver* rd);
  // virtual void Draw(RnrDriver* rd);
  // virtual void PostDraw(RnrDriver* rd);

  // virtual void Render(RnrDriver* rd);

}; // endclass Explosion_GL_Rnr

} // endnamespace gled

#endif
