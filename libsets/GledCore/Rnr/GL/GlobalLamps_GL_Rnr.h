// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef GledCore_GlobalLamps_GL_RNR_H
#define GledCore_GlobalLamps_GL_RNR_H

#include <Glasses/GlobalLamps.h>
#include <Rnr/GL/ZGlass_GL_Rnr.h>

namespace gled {

class Lamp_GL_Rnr;

class GlobalLamps_GL_Rnr : public ZGlass_GL_Rnr
{
private:
  void _init();

protected:
  GlobalLamps*		mGlobalLamps;
  std::list<Lamp_GL_Rnr*>	mLampsOn;

public:
  GlobalLamps_GL_Rnr(GlobalLamps* idol) :
    ZGlass_GL_Rnr(idol), mGlobalLamps(idol) { _init(); }
  virtual ~GlobalLamps_GL_Rnr() {}

  virtual void PreDraw(RnrDriver* rd);
  virtual void PostDraw(RnrDriver* rd);

}; // endclass GlobalLamps_GL_Rnr

} // endnamespace gled

#endif
