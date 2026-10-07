// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef Var1_CosmicBall_GL_RNR_H
#define Var1_CosmicBall_GL_RNR_H

#include <Glasses/CosmicBall.h>
#include <Rnr/GL/ZNode_GL_Rnr.h>

#include <GL/glew.h>

namespace gled {

class CosmicBall_GL_Rnr : public ZNode_GL_Rnr
{
private:
  void _init();

protected:
  CosmicBall*        mCosmicBall;

  static GLUquadricObj* sQuadric;

public:
  CosmicBall_GL_Rnr(CosmicBall* idol) :
    ZNode_GL_Rnr(idol), mCosmicBall(idol)
  { _init(); }

  //virtual void PreDraw(RnrDriver* rd);
  virtual void Draw(RnrDriver* rd);
  virtual void PostDraw(RnrDriver* rd);

}; // endclass CosmicBall_GL_Rnr

} // endnamespace gled

#endif
