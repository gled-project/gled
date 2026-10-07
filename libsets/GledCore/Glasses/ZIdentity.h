// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef GledCore_ZIdentity_H
#define GledCore_ZIdentity_H

#include <Glasses/ZGlass.h>

namespace gled {
class ZHashList;

class ZIdentity : public ZGlass
{
  MAC_RNR_FRIENDS(ZIdentity);

  friend class ZSunQueen;

private:
  void _init();

protected:
  ZLink<ZHashList>		mActiveMEEs;	// X{gS} L{}

  ZLink<ZMirFilter>		mAllowThis;	// X{gS} L{}

public:
  ZIdentity(const Text_t* n="ZIdentity", const Text_t* t=0) : ZGlass(n,t) { _init(); }

  virtual void AdEnlightenment();

#include "ZIdentity.h7"
  ClassDef(ZIdentity, 1); // Representation of a user identity
}; // endclass ZIdentity


} // endnamespace gled

#endif
