// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef Geom1_ZImage_GL_RNR_H
#define Geom1_ZImage_GL_RNR_H

#include <Glasses/ZImage.h>
#include <Rnr/GL/ZGlass_GL_Rnr.h>

#include <GL/glew.h>

namespace gled {

class ZImage_GL_Rnr : public ZGlass_GL_Rnr
{
private:
  void _init();

protected:
  ZImage*	mImage;
  TimeStamp_t	mStampTexture;
  GLuint	mTexture;

  void  check_rescale();
  Int_t nearest_pow2(Int_t d);

public:
  ZImage_GL_Rnr(ZImage* idol) : ZGlass_GL_Rnr(idol), mImage(idol) { _init(); }
  virtual ~ZImage_GL_Rnr();

  virtual void PreDraw(RnrDriver* rd);
  virtual void Draw(RnrDriver* rd);
  virtual void PostDraw(RnrDriver* rd);

  virtual void Triangulate(RnrDriver* rd);

  static Int_t sRescaleToPow2; // -1 unknown, 0 no rescale, 1

}; // endclass ZImage_GL_Rnr

} // endnamespace gled

#endif
