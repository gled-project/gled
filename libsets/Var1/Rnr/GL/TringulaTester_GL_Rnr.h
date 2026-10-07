// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef Var1_TringulaTester_GL_RNR_H
#define Var1_TringulaTester_GL_RNR_H

#include <Glasses/TringulaTester.h>
#include <Rnr/GL/ZNode_GL_Rnr.h>

namespace gled {

class TringulaTester_GL_Rnr : public ZNode_GL_Rnr
{
private:
  void _init();

protected:
  TringulaTester  *mTringulaTester;
  GLUquadricObj   *mQuadric;

public:
  TringulaTester_GL_Rnr(TringulaTester* idol);
  virtual ~TringulaTester_GL_Rnr();

  virtual void Draw(RnrDriver* rd);

}; // endclass TringulaTester_GL_Rnr

} // endnamespace gled

#endif
