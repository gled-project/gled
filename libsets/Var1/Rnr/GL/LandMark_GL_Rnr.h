// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef Var1_LandMark_GL_RNR_H
#define Var1_LandMark_GL_RNR_H

#include <Glasses/LandMark.h>
#include <Rnr/GL/Extendio_GL_Rnr.h>

namespace gled {

class LandMark_GL_Rnr : public Extendio_GL_Rnr
{
private:
  void _init();

protected:
  LandMark*        mLandMark;

  std::vector<Opcode::Point> mLandLinePoints;

public:
  LandMark_GL_Rnr(LandMark* idol) :
    Extendio_GL_Rnr(idol), mLandMark(idol)
  { _init(); }

  //virtual void PreDraw(RnrDriver* rd);
  //virtual void Draw(RnrDriver* rd);
  virtual void Render(RnrDriver* rd);
  virtual void PostDraw(RnrDriver* rd);

}; // endclass LandMark_GL_Rnr

} // endnamespace gled

#endif
