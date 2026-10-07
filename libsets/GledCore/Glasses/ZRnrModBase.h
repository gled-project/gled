// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef GledCore_ZRnrModBase_H
#define GledCore_ZRnrModBase_H

#include <Glasses/ZGlass.h>

namespace gled {

class ZRnrModBase : public ZGlass
{
  MAC_RNR_FRIENDS(ZRnrModBase);

public:
  enum Operation_e { O_Nop=-1, O_Off=0, O_On = 1 };

private:
  void _init();

protected:

public:
  ZRnrModBase(const Text_t* n="ZRnrModBase", const Text_t* t=0) : ZGlass(n,t) { _init(); }


#include "ZRnrModBase.h7"
  ClassDef(ZRnrModBase, 1);
}; // endclass ZRnrModBase


} // endnamespace gled

#endif
