// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef Tmp1_MonopodialTree_GL_RNR_H
#define Tmp1_MonopodialTree_GL_RNR_H

#include <Glasses/MonopodialTree.h>
#include <Rnr/GL/ParametricSystem_GL_Rnr.h>

namespace gled {

class MonopodialTree_GL_Rnr : public ParametricSystem_GL_Rnr
{
private:
  void _init();

protected:
  MonopodialTree*	mMonopodialTree;

public:
  MonopodialTree_GL_Rnr(MonopodialTree* idol);
  virtual ~MonopodialTree_GL_Rnr();
/*
  virtual void PreDraw(RnrDriver* rd);
  virtual void Draw(RnrDriver* rd);
  virtual void PostDraw(RnrDriver* rd);

  virtual void Render(RnrDriver* rd);
  
 */
  
  virtual void Triangulate(RnrDriver* rd);
}; // endclass MonopodialTree_GL_Rnr

} // endnamespace gled

#endif
