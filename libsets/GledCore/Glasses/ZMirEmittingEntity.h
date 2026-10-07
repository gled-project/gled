// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef GledCore_ZMirEmittingEntity_H
#define GledCore_ZMirEmittingEntity_H

#include <Glasses/ZGlass.h>
#include <Glasses/ZIdentity.h>
#include <Glasses/ZHashList.h>

namespace gled {

class SaturnInfo;

class ZMirEmittingEntity : public ZGlass
{
  MAC_RNR_FRIENDS(ZMirEmittingEntity);

  friend class Gled; friend class Saturn; friend class ZSunQueen;

private:
  void _init();

protected:
  TString           mLogin;             // X{GSR} 7 TextOut()
  ZLink<ZIdentity>  mPrimaryIdentity;   // X{gS} L{}
  ZLink<ZHashList>  mActiveIdentities;  // X{gS} L{}

public:
  ZMirEmittingEntity(const Text_t* n="ZMirEmittingEntity", const Text_t* t=0) :
    ZGlass(n,t) { _init(); }

  virtual void AdEnlightenment();

  virtual SaturnInfo* HostingSaturn() = 0;
  virtual void Message(const TString& s) {} // X{E}
  virtual void Warning(const TString& s) {} // X{E} T{MEE::Self}
  virtual void Error  (const TString& s) {} // X{E} T{MEE::Self}

  Bool_t HasIdentity(ZIdentity* ident);

#include "ZMirEmittingEntity.h7"
  ClassDef(ZMirEmittingEntity, 1);
}; // endclass ZMirEmittingEntity


} // endnamespace gled

#endif
