// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef GledCore_ZRnrModBase_GL_RNR_H
#define GledCore_ZRnrModBase_GL_RNR_H

#include <Glasses/ZRnrModBase.h>
#include <Rnr/GL/ZGlass_GL_Rnr.h>

namespace gled {

class ZRnrModBase_GL_Rnr : public ZGlass_GL_Rnr
{
private:
  void _init();

protected:
  ZRnrModBase* mZRnrModBase;
  RnrMod*      mRnrMod;

  virtual void update_tring_stamp(RnrDriver* rd);

public:
  ZRnrModBase_GL_Rnr(ZRnrModBase* idol) :
    ZGlass_GL_Rnr(idol), mZRnrModBase(idol)
  { _init(); }
  virtual ~ZRnrModBase_GL_Rnr();

}; // endclass ZRnrModBase_GL_Rnr

} // endnamespace gled

#endif
