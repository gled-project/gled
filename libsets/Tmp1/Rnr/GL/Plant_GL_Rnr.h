// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef Tmp1_Plant_GL_RNR_H
#define Tmp1_Plant_GL_RNR_H

#include <Glasses/Plant.h>
#include <Rnr/GL/Weed_GL_Rnr.h>

namespace gled {

class Plant_GL_Rnr : public Weed_GL_Rnr
{
private:
  void _init();
  Plant*	mPlant;
  GLUquadricObj* mQuadric; 
  
protected:  
  virtual void DrawStep(Turtle& t) const;
  virtual void DrawLeaf(Turtle& t) const;
  virtual void DrawFlower(Turtle& t) const;
  virtual void DecreaseWidth(Turtle& t) const;

  virtual void SetStepSize();
   
public:
  Plant_GL_Rnr(Plant* idol);
  virtual ~Plant_GL_Rnr();
  
  virtual void Render(RnrDriver* rd);
}; // endclass Plant_GL_Rnr

} // endnamespace gled

#endif
