// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef Var1_SolarSystem_GL_RNR_H
#define Var1_SolarSystem_GL_RNR_H

#include <Glasses/SolarSystem.h>
#include <Rnr/GL/ZNode_GL_Rnr.h>

namespace gled {

class SolarSystem_GL_Rnr : public ZNode_GL_Rnr
{
private:
  void _init();

protected:
  SolarSystem*  mSolarSystem;

public:
  SolarSystem_GL_Rnr(SolarSystem* idol) :
    ZNode_GL_Rnr(idol), mSolarSystem(idol)
  { _init(); }

  //virtual void PreDraw(RnrDriver* rd);
  virtual void Draw(RnrDriver* rd);
  //virtual void PostDraw(RnrDriver* rd);

}; // endclass SolarSystem_GL_Rnr

} // endnamespace gled

#endif
