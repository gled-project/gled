// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef GledCore_ZFireQueen_H
#define GledCore_ZFireQueen_H

#include <Glasses/ZQueen.h>

namespace gled {
class ZEunuch;

class ZFireQueen : public ZQueen
{
  MAC_RNR_FRIENDS(ZFireQueen);

private:
  void _init();

protected:
  ZLink<ZHashList>	mEunuchs;	// X{gS} L{}

  virtual void bootstrap();

public:
  ZFireQueen(const Text_t* n="ZFireQueen", const Text_t* t=0) :
    ZQueen(n, t) { _init(); }
  ZFireQueen(ID_t span, const Text_t* n="ZFireQueen", const Text_t* t=0) :
    ZQueen(span, n, t) { _init(); }

  // ID & Lens management
  virtual ZGlass* DemangleID(ID_t id);

#include "ZFireQueen.h7"
  ClassDef(ZFireQueen, 1);
}; // endclass ZFireQueen


} // endnamespace gled

#endif
