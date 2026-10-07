// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef GledCore_ZRnrModList_H
#define GledCore_ZRnrModList_H

#include <Glasses/ZHashList.h>

namespace gled {

class ZRnrModList : public ZHashList
{
  MAC_RNR_FRIENDS(ZRnrModList);

private:
  void _init();

protected:

public:
  ZRnrModList(const Text_t* n="ZRnrModList", const Text_t* t=0) :
    ZHashList(n,t) { _init(); }


#include "ZRnrModList.h7"
  ClassDef(ZRnrModList, 1); // List of lenses that modify renderer-state
}; // endclass ZRnrModList


} // endnamespace gled

#endif
