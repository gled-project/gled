// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef Gled_ZGod_H
#define Gled_ZGod_H

#include <Glasses/ZHashList.h>

namespace gled {

class ZGod : public ZHashList {
  MAC_RNR_FRIENDS(ZGod);
private:
protected:
public:
  ZGod(const Text_t* n="ZGod", const Text_t* t=0) : ZHashList(n,t) {}


#include "ZGod.h7"
  ClassDef(ZGod, 1);
}; // endclass ZGod


} // endnamespace gled

#endif
