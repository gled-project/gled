// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef Numerica_Spheror_GL_RNR_H
#define Numerica_Spheror_GL_RNR_H

#include <Glasses/Spheror.h>
#include <Rnr/GL/ZNode_GL_Rnr.h>

namespace gled {

class Spheror_GL_Rnr : public ZNode_GL_Rnr {
private:

protected:
  Spheror*	mSpheror;

public:
  Spheror_GL_Rnr(Spheror* idol) : ZNode_GL_Rnr(idol), mSpheror(idol) {}

  virtual void Draw(RnrDriver* rd);

}; // endclass Spheror_GL_Rnr

} // endnamespace gled

#endif
