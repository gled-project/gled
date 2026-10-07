// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef GledCore_ZGlLightModel_GL_RNR_H
#define GledCore_ZGlLightModel_GL_RNR_H

#include <Glasses/ZGlLightModel.h>
#include <Rnr/GL/ZRnrModBase_GL_Rnr.h>

namespace gled {

class ZGlLightModel_GL_Rnr : public ZRnrModBase_GL_Rnr {
private:
  void _init();

protected:
  ZGlLightModel*	mZGlLightModel;

public:
  ZGlLightModel_GL_Rnr(ZGlLightModel* idol) : ZRnrModBase_GL_Rnr(idol), mZGlLightModel(idol) {}

  virtual void PreDraw(RnrDriver* rd);
  virtual void Draw(RnrDriver* rd);
  virtual void PostDraw(RnrDriver* rd);

  void SetupGL();

}; // endclass ZGlLightModel_GL_Rnr

} // endnamespace gled

#endif
