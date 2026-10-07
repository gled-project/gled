// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef Geom1_ZGlShader_GL_RNR_H
#define Geom1_ZGlShader_GL_RNR_H

#include <Glasses/ZGlShader.h>
#include <Rnr/GL/ZGlass_GL_Rnr.h>

namespace gled {

class ZGlShader_GL_Rnr : public ZGlass_GL_Rnr
{
private:
  void _init();

protected:
  ZGlShader*	mZGlShader;

  Bool_t        bRecompile;
  GLuint	mShaderID;

public:
  ZGlShader_GL_Rnr(ZGlShader* idol);
  virtual ~ZGlShader_GL_Rnr();

  virtual void AbsorbRay(Ray& ray);

  virtual GLuint AssertShader();
  virtual GLuint Compile();
}; // endclass ZGlShader_GL_Rnr

} // endnamespace gled

#endif
