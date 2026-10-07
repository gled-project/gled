// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef Geom1_WSSeed_GL_RNR_H
#define Geom1_WSSeed_GL_RNR_H

#include <Glasses/WSSeed.h>
#include <Rnr/GL/ZNode_GL_Rnr.h>

namespace gled {

class WSSeed_GL_Rnr : public ZNode_GL_Rnr
{
protected:
  WSSeed*	mWSSeed;

  void vert(WSPoint* f, Float_t t);

public:
  WSSeed_GL_Rnr(WSSeed* idol) : ZNode_GL_Rnr(idol), mWSSeed(idol) {}

  virtual void Draw(RnrDriver* rd);

  virtual void Render(RnrDriver* rd);
  virtual void Triangulate(RnrDriver* rd);

}; // endclass WSSeed_GL_Rnr

} // endnamespace gled

#endif
