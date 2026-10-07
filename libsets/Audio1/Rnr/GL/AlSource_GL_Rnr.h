// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef Audio1_AlSource_GL_RNR_H
#define Audio1_AlSource_GL_RNR_H

#include <Glasses/AlSource.h>
#include <Rnr/GL/ZNode_GL_Rnr.h>

namespace gled {

class AlSource_GL_Rnr : public ZNode_GL_Rnr
{
protected:
  AlSource*	mAlSource;

public:
  AlSource_GL_Rnr(AlSource* idol) :
    ZNode_GL_Rnr(idol), mAlSource(idol)
  {}

  //virtual void PreDraw(RnrDriver* rd);
  virtual void Draw(RnrDriver* rd);
  //virtual void PostDraw(RnrDriver* rd);

}; // endclass AlSource_GL_Rnr

} // endnamespace gled

#endif
