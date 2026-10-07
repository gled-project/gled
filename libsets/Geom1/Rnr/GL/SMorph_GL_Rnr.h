// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef Geom1_SMorph_GL_Rnr_H
#define Geom1_SMorph_GL_Rnr_H

#include <Glasses/SMorph.h>
#include <Rnr/GL/ZNode_GL_Rnr.h>

namespace gled {

class SMorph_GL_Rnr : public ZNode_GL_Rnr {
private:

protected:
  SMorph*	mSMorph;

public:
  SMorph_GL_Rnr(SMorph* m) : ZNode_GL_Rnr(m), mSMorph(m) {}

  virtual void Render(RnrDriver* rd);
  virtual void Triangulate(RnrDriver* rd);

}; // endclass SMorph_GL_Rnr

} // endnamespace gled

#endif
