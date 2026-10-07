// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef GledCore_ZGlMaterial_GL_RNR_H
#define GledCore_ZGlMaterial_GL_RNR_H

#include <Glasses/ZGlMaterial.h>
#include <Rnr/GL/ZRnrModBase_GL_Rnr.h>

namespace gled {

class ZGlMaterial_GL_Rnr : public ZRnrModBase_GL_Rnr {
private:
  void _init();

protected:
  ZGlMaterial*	mZGlMaterial;

public:
  ZGlMaterial_GL_Rnr(ZGlMaterial* idol) : ZRnrModBase_GL_Rnr(idol), mZGlMaterial(idol) {}

  virtual void PreDraw(RnrDriver* rd);
  virtual void Draw(RnrDriver* rd);
  virtual void PostDraw(RnrDriver* rd);

  void SetupGL();

}; // endclass ZGlMaterial_GL_Rnr

} // endnamespace gled

#endif
