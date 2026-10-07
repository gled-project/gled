// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef GledCore_Lamp_GL_RNR_H
#define GledCore_Lamp_GL_RNR_H

#include <Glasses/Lamp.h>
#include <Rnr/GL/ZNode_GL_Rnr.h>

namespace gled {

class Lamp_GL_Rnr : public ZNode_GL_Rnr {
private:

protected:
  Lamp*		mLamp;
  Int_t		mLampID;	// X{G}

public:
  Lamp_GL_Rnr(Lamp* idol) : ZNode_GL_Rnr(idol), mLamp(idol), mLampID(-1) {}

  virtual void Draw(RnrDriver* rd);
  virtual void CleanUp(RnrDriver* rd);

  int LampOn(RnrDriver* rd, const ZTrans* t=0);
  int LampOff(RnrDriver* rd);

#include "Lamp_GL_Rnr.h7"
}; // endclass Lamp_GL_Rnr

} // endnamespace gled

#endif
