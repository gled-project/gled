// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef Geom1_Amphitheatre_GL_RNR_H
#define Geom1_Amphitheatre_GL_RNR_H

#include <Glasses/Amphitheatre.h>
#include <Rnr/GL/ZNode_GL_Rnr.h>

namespace gled {

class Amphitheatre_GL_Rnr : public ZNode_GL_Rnr {
private:
  void _init();

protected:
  Amphitheatre*	 mAmphitheatre;
  GLUquadricObj* mQuadric;

public:
  Amphitheatre_GL_Rnr(Amphitheatre* idol) :
    ZNode_GL_Rnr(idol), mAmphitheatre(idol) { _init(); }

  //virtual void PreDraw(RnrDriver* rd);
  virtual void Draw(RnrDriver* rd);
  //virtual void PostDraw(RnrDriver* rd);

}; // endclass Amphitheatre_GL_Rnr

} // endnamespace gled

#endif
