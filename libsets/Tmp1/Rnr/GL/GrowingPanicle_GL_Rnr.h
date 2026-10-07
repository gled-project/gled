// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef Tmp1_GrowingPanicle_GL_RNR_H
#define Tmp1_GrowingPanicle_GL_RNR_H

#include <Glasses/GrowingPanicle.h>
#include <Rnr/GL/GrowingPlant_GL_Rnr.h>

namespace gled {

class GrowingPanicle_GL_Rnr : public GrowingPlant_GL_Rnr
{
private:
  void _init();

protected:
  GrowingPanicle*	mGrowingPanicle;
  virtual void DrawSymbol(Turtle& turtle, GrowingPlant::Segment& p);
  virtual void DrawStep(Turtle& turtle, GrowingPlant::Segment& p);
  
  
  virtual void DrawSignal(float);
  
public:
  GrowingPanicle_GL_Rnr(GrowingPanicle* idol);
  virtual ~GrowingPanicle_GL_Rnr();

}; // endclass GrowingPanicle_GL_Rnr

} // endnamespace gled

#endif
