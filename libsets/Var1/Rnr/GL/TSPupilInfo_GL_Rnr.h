// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef Var1_TSPupilInfo_GL_RNR_H
#define Var1_TSPupilInfo_GL_RNR_H

#include <Glasses/TSPupilInfo.h>
#include <Rnr/GL/PupilInfo_GL_Rnr.h>

namespace gled {

class TSPupilInfo_GL_Rnr : public PupilInfo_GL_Rnr
{
private:
  void _init();

protected:
  TSPupilInfo*        mTSPupilInfo;

public:
  TSPupilInfo_GL_Rnr(TSPupilInfo* idol);
  virtual ~TSPupilInfo_GL_Rnr();

  virtual int  Handle(RnrDriver* rd, Fl_Event& ev);

  // virtual void PreDraw(RnrDriver* rd);
  // virtual void Draw(RnrDriver* rd);
  // virtual void PostDraw(RnrDriver* rd);
  // virtual void Render(RnrDriver* rd);

}; // endclass TSPupilInfo_GL_Rnr

} // endnamespace gled

#endif
