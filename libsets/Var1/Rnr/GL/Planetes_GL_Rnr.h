// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef Var1_Planetes_GL_RNR_H
#define Var1_Planetes_GL_RNR_H

#include <Glasses/Planetes.h>
#include <Rnr/GL/ZNode_GL_Rnr.h>

namespace gled {

class Planetes_GL_Rnr : public ZNode_GL_Rnr
{
private:
  void _init();

protected:
  Planetes*        mPlanetes;

public:
  Planetes_GL_Rnr(Planetes* idol);
  virtual ~Planetes_GL_Rnr();

  // virtual void PreDraw(RnrDriver* rd);
  virtual void Draw(RnrDriver* rd);
  // virtual void PostDraw(RnrDriver* rd);

  virtual void Render(RnrDriver* rd);

}; // endclass Planetes_GL_Rnr

} // endnamespace gled

#endif
