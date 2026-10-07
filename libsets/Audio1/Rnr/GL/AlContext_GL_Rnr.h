// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef Audio1_AlContext_GL_RNR_H
#define Audio1_AlContext_GL_RNR_H

#include <Glasses/AlContext.h>
#include <Rnr/GL/ZNode_GL_Rnr.h>

namespace gled {

class AlContext_GL_Rnr : public ZNode_GL_Rnr {
private:
  void _init();

protected:
  AlContext*	mAlContext;

public:
  AlContext_GL_Rnr(AlContext* idol) :
    ZNode_GL_Rnr(idol), mAlContext(idol)
  { _init(); }

  virtual void PreDraw(RnrDriver* rd);
  virtual void Draw(RnrDriver* rd);
  virtual void PostDraw(RnrDriver* rd);

}; // endclass AlContext_GL_Rnr

} // endnamespace gled

#endif
