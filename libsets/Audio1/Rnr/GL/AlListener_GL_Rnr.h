// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef Audio1_AlListener_GL_RNR_H
#define Audio1_AlListener_GL_RNR_H

#include <Glasses/AlListener.h>
#include <Rnr/GL/ZNode_GL_Rnr.h>

namespace gled {

class AlListener_GL_Rnr : public ZNode_GL_Rnr
{
private:
  void _init();

protected:
  AlListener*	mAlListener;

public:
  AlListener_GL_Rnr(AlListener* idol) :
    ZNode_GL_Rnr(idol), mAlListener(idol)
  { _init(); }

  //virtual void PreDraw(RnrDriver* rd);
  virtual void Draw(RnrDriver* rd);
  //virtual void PostDraw(RnrDriver* rd);

}; // endclass AlListener_GL_Rnr

} // endnamespace gled

#endif
