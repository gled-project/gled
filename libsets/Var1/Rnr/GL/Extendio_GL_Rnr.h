// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef Var1_Extendio_GL_RNR_H
#define Var1_Extendio_GL_RNR_H

#include <Glasses/Extendio.h>
#include <Rnr/GL/ZGlass_GL_Rnr.h>

namespace gled {

class Extendio_GL_Rnr : public ZGlass_GL_Rnr
{
private:
  void _init();

protected:
  Extendio*        mExtendio;
  Bool_t        bRnrBBoxes;

public:
  Extendio_GL_Rnr(Extendio* idol);
  virtual ~Extendio_GL_Rnr();

  virtual void PreDraw(RnrDriver* rd);
  virtual void Draw(RnrDriver* rd);
  virtual void Render(RnrDriver* rd);
  virtual void PostDraw(RnrDriver* rd);

}; // endclass Extendio_GL_Rnr

} // endnamespace gled

#endif
