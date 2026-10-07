// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef Tmp1_ParametricSystem_GL_RNR_H
#define Tmp1_ParametricSystem_GL_RNR_H

#include "TAttBBox.h"
#include <Glasses/ParametricSystem.h>
#include <Rnr/GL/ZNode_GL_Rnr.h>

class TRandom;

namespace gled {

class ZTrans;

class ParametricSystem_GL_Rnr : public ZNode_GL_Rnr,
                                public TAttBBox
{  
private:
  void _init();
  
protected:
  
  struct Turtle {
    ZTrans mTrans;
    float  mWidth;
    float  mLength;
    bool   mChangedWidth;
  };
  
  ParametricSystem*	mPS;
  GLUquadricObj*    mQuadric;
  float             mScale;
  TRandom*          mRandom;
  UInt_t            mSeed;
   
  virtual void ProcessExpression(bool);
  virtual void DrawStep(Turtle& turtle, TwoParam& p, bool);
    
  
  virtual void ComputeBBox() {}
  float GetMaxExtend();

public:
  ParametricSystem_GL_Rnr(ParametricSystem* idol);
  virtual ~ParametricSystem_GL_Rnr();
  
  virtual void Render(RnrDriver* rd);
  virtual void Triangulate(RnrDriver* rd);
  
}; // endclass ParametricSystem_GL_Rnr

} // endnamespace gled

#endif
