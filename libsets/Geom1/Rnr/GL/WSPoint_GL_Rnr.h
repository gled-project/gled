// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef Geom1_WSPoint_GL_RNR_H
#define Geom1_WSPoint_GL_RNR_H

#include <Glasses/WSPoint.h>
#include <Rnr/GL/ZNode_GL_Rnr.h>

namespace gled {

class WSPoint_GL_Rnr : public ZNode_GL_Rnr {
private:

protected:
  WSPoint*	mWSPoint;

public:
  WSPoint_GL_Rnr(WSPoint* idol) : ZNode_GL_Rnr(idol), mWSPoint(idol) {}

  virtual void Draw(RnrDriver* rd);

}; // endclass WSPoint_GL_Rnr

} // endnamespace gled

#endif
