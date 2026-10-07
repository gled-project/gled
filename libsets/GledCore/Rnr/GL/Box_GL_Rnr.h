// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef GledCore_Box_GL_RNR_H
#define GledCore_Box_GL_RNR_H

#include <Glasses/Box.h>
#include <Rnr/GL/ZNode_GL_Rnr.h>

namespace gled {

class Box_GL_Rnr : public ZNode_GL_Rnr {
private:

protected:
  Box*	mBox;

public:
  Box_GL_Rnr(Box* idol) : ZNode_GL_Rnr(idol), mBox(idol) {}

  virtual void Draw(RnrDriver* rd);

}; // endclass Box_GL_Rnr

} // endnamespace gled

#endif
