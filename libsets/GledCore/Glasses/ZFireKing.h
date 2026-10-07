// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef GledCore_ZFireKing_H
#define GledCore_ZFireKing_H

#include <Glasses/ZKing.h>

namespace gled {
class ZQueen;

class ZFireKing : public ZKing {
  MAC_RNR_FRIENDS(ZFireKing);

private:
  void _init();

protected:

public:
  ZFireKing(const Text_t* n="ZFireKing", const Text_t* t=0) :
    ZKing(n,t) { _init(); }
  ZFireKing(ID_t min, ID_t max, const Text_t* n="ZKing", const Text_t* t=0) :
    ZKing(min,max,n,t) { _init(); }

  virtual void Enthrone(ZQueen* queen);

  // Mirroring of queens.
  void RequestQueenMirroring(ZQueen* queen_to_mirror);	// X{E} C{1}
  void RequestQueenShattering(ZQueen* queen_to_leave);	// X{E} C{1}

#include "ZFireKing.h7"
  ClassDef(ZFireKing, 1); // Ruler of fire space and queen mirroring broker.
}; // endclass ZFireKing


} // endnamespace gled

#endif
