// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef Var1_PSMark_GL_RNR_H
#define Var1_PSMark_GL_RNR_H

#include <Glasses/PSMark.h>
#include <Rnr/GL/ZNode_GL_Rnr.h>

namespace gled {

class PSMark_GL_Rnr : public ZNode_GL_Rnr {
private:
  void _init();

protected:
  PSMark*        mPSMark;

public:
  PSMark_GL_Rnr(PSMark* idol) :
    ZNode_GL_Rnr(idol), mPSMark(idol)
  { _init(); }

  virtual void Render(RnrDriver* rd);

}; // endclass PSMark_GL_Rnr

} // endnamespace gled

#endif
