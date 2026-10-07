// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef Tmp1_DibotryoidHerb_GL_RNR_H
#define Tmp1_DibotryoidHerb_GL_RNR_H

#include <Glasses/DibotryoidHerb.h>
#include <Rnr/GL/GrowingPlant_GL_Rnr.h>

namespace gled {

class DibotryoidHerb_GL_Rnr : public GrowingPlant_GL_Rnr
{
private:
  void _init();

protected:
  DibotryoidHerb*	mDibotryoidHerb;
  
  virtual void DrawSymbol(Turtle& turtle, GrowingPlant::Segment& p);
  virtual void DrawStep(Turtle& turtle, GrowingPlant::Segment& p);
  
public:
  DibotryoidHerb_GL_Rnr(DibotryoidHerb* idol);
  virtual ~DibotryoidHerb_GL_Rnr();
  
    
/*
  virtual void PreDraw(RnrDriver* rd);
  virtual void Draw(RnrDriver* rd);
  virtual void PostDraw(RnrDriver* rd);
 virtual void Render(RnrDriver* rd);
 
 */

}; // endclass DibotryoidHerb_GL_Rnr

} // endnamespace gled

#endif
