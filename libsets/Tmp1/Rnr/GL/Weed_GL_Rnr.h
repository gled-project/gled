// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef Tmp1_Weed_GL_RNR_H
#define Tmp1_Weed_GL_RNR_H

#include <Glasses/Weed.h>
#include <Glasses/ZVector.h>
#include <Rnr/GL/ZNode_GL_Rnr.h>

#include <stack>
#include <vector>
#include <map>

namespace gled {

class Weed_GL_Rnr : public ZNode_GL_Rnr
{
protected:
  class Turtle {
  public:
    ZTrans   trans;
    float    lineWidth;
    Turtle() : lineWidth(3.f) {}
  };
  
  void _init();  
  
  void ProcessExpression() const;
  
  virtual void SetStepSize();
  virtual void DrawStep(Turtle& t) const;
  virtual void DrawLeaf(Turtle& t) const;
  virtual void DrawFlower(Turtle& t) const;
  virtual void DecreaseWidth(Turtle& t) const;
    
  Weed*	       mWeed;
  float        mStepSize;
  
public:
  Weed_GL_Rnr(Weed* idol);
  virtual ~Weed_GL_Rnr();
  
  virtual void Render(RnrDriver* rd);
  virtual void Triangulate(RnrDriver* rd);
  
  void DumpInfo() const;
  
}; // endclass Weed_GL_Rnr

} // endnamespace gled

#endif
