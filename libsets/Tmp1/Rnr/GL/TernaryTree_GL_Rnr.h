// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef Tmp1_TernaryTree_GL_RNR_H
#define Tmp1_TernaryTree_GL_RNR_H

#include <Glasses/TernaryTree.h>
#include <Rnr/GL/MonopodialTree_GL_Rnr.h>

namespace gled {

class TernaryTree_GL_Rnr : public MonopodialTree_GL_Rnr
{
private:
  void _init();

protected:
  TernaryTree*	mTT;
  virtual void DrawStep(Turtle& turtle, TwoParam& p, bool);
  virtual void ProcessExpression(bool);
  
public:
  TernaryTree_GL_Rnr(TernaryTree* idol);
  virtual ~TernaryTree_GL_Rnr();

}; // endclass TernaryTree_GL_Rnr

} // endnamespace gled

#endif
