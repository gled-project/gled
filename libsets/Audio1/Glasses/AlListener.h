// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef Audio1_AlListener_H
#define Audio1_AlListener_H

#include <Glasses/ZNode.h>

namespace gled {

class AlListener : public ZNode
{
  MAC_RNR_FRIENDS(AlListener);

public:
  enum LocationType_e { LT_Camera, LT_Absolute };

private:
  void _init();

protected:
  LocationType_e mLocationType; // X{GS} 7 PhonyEnum()

  Float_t        mGain; // X{GS} Ray{Source} 7 Value(-range=>[0,100,1,1000])

public:
  AlListener(const Text_t* n="AlListener", const Text_t* t=0) :
    ZNode(n,t) { _init(); }

  void EmitSourceRay();

#include "AlListener.h7"
  ClassDef(AlListener, 1);
}; // endclass AlListener


} // endnamespace gled

#endif
