// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef Tmp1_GrowingPlant_GL_RNR_H
#define Tmp1_GrowingPlant_GL_RNR_H

#include "TAttBBox.h"
#include <Glasses/GrowingPlant.h>
#include <Rnr/GL/ZNode_GL_Rnr.h>

namespace gled {

class GrowingPlant_GL_Rnr : public ZNode_GL_Rnr, public TAttBBox
{
protected:
  void _init();
  
  struct Turtle {
    ZTrans mTrans;
  };
  
  GrowingPlant*	    mModel;
  GLUquadricObj*    mQuadric;
  bool              mLighting;
  
  bool              mIAsForward;
  
  virtual void ProcessExpression(RnrDriver* rd);
  virtual void DrawStep(Turtle& turtle, GrowingPlant::Segment& p);
  virtual void DrawSymbol(Turtle& turtle, GrowingPlant::Segment& p) ;
  
  virtual void ComputeBBox() {}  
public:
  GrowingPlant_GL_Rnr(GrowingPlant* idol);
  virtual ~GrowingPlant_GL_Rnr();
  
  virtual void Render(RnrDriver* rd);
  virtual void Triangulate(RnrDriver* rd);
  
  
  virtual void HandlePick(RnrDriver* rd, lNSE_t& ns, lNSE_i nsi);

}; // endclass GrowingPlant_GL_Rnr

} // endnamespace gled

#endif
