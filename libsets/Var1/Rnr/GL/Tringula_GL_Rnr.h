// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef Var1_Tringula_GL_RNR_H
#define Var1_Tringula_GL_RNR_H

#include <Glasses/Tringula.h>
#include <Rnr/GL/ZNode_GL_Rnr.h>

namespace gled {

class Tringula_GL_Rnr : public ZNode_GL_Rnr
{
private:
  void _init();

protected:
  Tringula*         mTringula;
  TimeStamp_t    mMeshTringStamp;

public:
  Tringula_GL_Rnr(Tringula* idol) :
    ZNode_GL_Rnr(idol), mTringula(idol)
  { _init(); }
  virtual ~Tringula_GL_Rnr();

  void RenderExtendio(RnrDriver* rd, Extendio* dyno);
  void RenderExtendios(RnrDriver* rd, AList* list);

  virtual void Draw(RnrDriver* rd);
  virtual void Render(RnrDriver* rd);

}; // endclass Tringula_GL_Rnr

} // endnamespace gled

#endif
