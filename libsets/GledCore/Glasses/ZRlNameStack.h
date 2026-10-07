// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef GledCore_ZRlNameStack_H
#define GledCore_ZRlNameStack_H

#include <Glasses/ZRnrModBase.h>

namespace gled {

class ZRlNameStack : public ZRnrModBase {
  // 7777 RnrCtrl(RnrBits(0,4,0,0))
  MAC_RNR_FRIENDS(ZRlNameStack);

private:
  void _init();

protected:
  ZRnrModBase::Operation_e mNameStackOp;  // X{GS} 7 PhonyEnum()
  Bool_t                   bClearStack;   // X{GS} 7 Bool(-join=>1)
  Bool_t                   bRestoreStack; // X{GS} 7 Bool()

public:
  ZRlNameStack(const Text_t* n="ZRlNameStack", const Text_t* t=0) :
    ZRnrModBase(n,t) { _init(); }

#include "ZRlNameStack.h7"
  ClassDef(ZRlNameStack, 1);
}; // endclass ZRlNameStack


} // endnamespace gled

#endif
