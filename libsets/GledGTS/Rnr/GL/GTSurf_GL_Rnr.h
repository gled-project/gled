// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef GTS_GTSurf_GL_RNR_H
#define GTS_GTSurf_GL_RNR_H

#include <Glasses/GTSurf.h>
#include <Rnr/GL/ZNode_GL_Rnr.h>

namespace gled {

class GTSurf_GL_Rnr : public ZNode_GL_Rnr
{
protected:
  GTSurf*	mGTSurf;

public:
  GTSurf_GL_Rnr(GTSurf* idol) : ZNode_GL_Rnr(idol), mGTSurf(idol) {}

  virtual void Draw(RnrDriver* rd);
  virtual void Render(RnrDriver* rd);

}; // endclass GTSurf_GL_Rnr

} // endnamespace gled

#endif
