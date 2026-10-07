// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef GledCore_PupilInfo_GL_RNR_H
#define GledCore_PupilInfo_GL_RNR_H

#include <Glasses/PupilInfo.h>
#include <Rnr/GL/ZGlass_GL_Rnr.h>

namespace gled {

class PupilInfo_GL_Rnr : public ZGlass_GL_Rnr
{
private:
  void _init();

protected:
  PupilInfo    *mPupilInfo;

public:
  PupilInfo_GL_Rnr(PupilInfo* idol) : ZGlass_GL_Rnr(idol), mPupilInfo(idol)
  { _init();}

  virtual void InitRendering(RnrDriver* rd);

  virtual int  Handle(RnrDriver* rd, Fl_Event& ev);

  virtual void PreDraw(RnrDriver* rd);
  virtual void Draw(RnrDriver* rd);
  virtual void PostDraw(RnrDriver* rd);

}; // endclass PupilInfo_GL_Rnr

} // endnamespace gled

#endif
