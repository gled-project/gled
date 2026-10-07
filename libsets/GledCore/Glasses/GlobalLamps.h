// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef GledCore_GlobalLamps_H
#define GledCore_GlobalLamps_H

#include <Glasses/ZHashList.h>
#include <Glasses/Lamp.h>

namespace gled {

class GlobalLamps : public ZHashList
{
  MAC_RNR_FRIENDS(GlobalLamps);

private:
  void _init();

protected:

public:
  GlobalLamps(const Text_t* n="GlobalLamps", const Text_t* t=0) : ZHashList(n,t) { _init(); }

#include "GlobalLamps.h7"
  ClassDef(GlobalLamps, 1);
}; // endclass GlobalLamps


} // endnamespace gled

#endif
