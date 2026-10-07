// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef GledCore_ZGroupIdentity_H
#define GledCore_ZGroupIdentity_H

#include <Glasses/ZIdentity.h>

namespace gled {

class ZGroupIdentity : public ZIdentity {
  MAC_RNR_FRIENDS(ZGroupIdentity);

  friend class ZSunQueen;

private:
  void _init();

protected:

public:
  ZGroupIdentity(const Text_t* n="ZGroupIdentity", const Text_t* t=0) :
    ZIdentity(n,t) { _init(); }

#include "ZGroupIdentity.h7"
  ClassDef(ZGroupIdentity, 1); // Representation of a group identity
}; // endclass ZGroupIdentity


} // endnamespace gled

#endif
