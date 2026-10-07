// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef Geom1_GLTesterOne_GL_RNR_H
#define Geom1_GLTesterOne_GL_RNR_H

#include <Glasses/GLTesterOne.h>
#include <Rnr/GL/ZNode_GL_Rnr.h>

namespace gled {

class GLTesterOne_GL_Rnr : public ZNode_GL_Rnr {
private:
  void _init();

protected:
  GLTesterOne*	mGLTesterOne;

public:
  GLTesterOne_GL_Rnr(GLTesterOne* idol) :
    ZNode_GL_Rnr(idol), mGLTesterOne(idol)
  { _init(); }

  virtual void Draw(RnrDriver* rd);

  virtual void Render(RnrDriver* rd);

}; // endclass GLTesterOne_GL_Rnr

} // endnamespace gled

#endif
