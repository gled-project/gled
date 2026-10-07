// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef @LIBSET@_@CLASS@_GL_RNR_H
#define @LIBSET@_@CLASS@_GL_RNR_H

#include <Glasses/@CLASS@.h>
#include <Rnr/GL/@BASE@_GL_Rnr.h>

namespace gled {

class @CLASS@_GL_Rnr : public @BASE@_GL_Rnr
{
private:
  void _init();

protected:
  @CLASS@*	m@CLASS@;

public:
  @CLASS@_GL_Rnr(@CLASS@* idol);
  virtual ~@CLASS@_GL_Rnr();

  // PreDraw() and PostDraw() of the base set up and restore the GL state,
  // e.g. the transformation of a ZNode; override them only to extend them.
  virtual void Draw(RnrDriver* rd);

}; // endclass @CLASS@_GL_Rnr

} // endnamespace gled

#endif
