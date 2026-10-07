// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef GledCore_Cylinder_GL_RNR_H
#define GledCore_Cylinder_GL_RNR_H

#include <Glasses/Cylinder.h>
#include <Rnr/GL/ZNode_GL_Rnr.h>

#include <GL/glew.h>

namespace gled {

class Cylinder_GL_Rnr : public ZNode_GL_Rnr {
 private:
  void _init();

 protected:
  Cylinder*      mCylinder;
  GLUquadricObj* mQuadric;

 public:
  Cylinder_GL_Rnr(Cylinder* idol) :
    ZNode_GL_Rnr(idol), mCylinder(idol) { _init(); }
  virtual ~Cylinder_GL_Rnr();

  virtual void Render(RnrDriver* rd);
};

} // endnamespace gled

#endif
