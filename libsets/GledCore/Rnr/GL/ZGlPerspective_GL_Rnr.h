// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef GledCore_ZGlPerspective_GL_RNR_H
#define GledCore_ZGlPerspective_GL_RNR_H

#include <Glasses/ZGlPerspective.h>
#include <Rnr/GL/ZRnrModBase_GL_Rnr.h>

namespace gled {

class ZGlPerspective_GL_Rnr : public ZRnrModBase_GL_Rnr
{
private:
  void _init();

protected:
  ZGlPerspective*	mZGlPerspective;

  void setup_matrices(RnrDriver* rd, bool push_p);

public:
  ZGlPerspective_GL_Rnr(ZGlPerspective* idol) :
    ZRnrModBase_GL_Rnr(idol), mZGlPerspective(idol)
  { _init(); }

  virtual void PreDraw(RnrDriver* rd);
  virtual void Draw(RnrDriver* rd);
  virtual void PostDraw(RnrDriver* rd);

}; // endclass ZGlPerspective_GL_Rnr

} // endnamespace gled

#endif
