// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef GledCore_Scene_H
#define GledCore_Scene_H

#include <Glasses/ZNode.h>
#include <Glasses/GlobalLamps.h>

namespace gled {

class Scene : public ZNode {
  MAC_RNR_FRIENDS(Scene);
private:
  void _init();

protected:
  ZLink<GlobalLamps>		mGlobLamps; // X{gS} L{} RnrBits{3,0,6,0}

public:
  Scene(const Text_t* n="Scene", const Text_t* t=0) : ZNode(n,t) { _init(); }

  virtual void AdEnlightenment();

#include "Scene.h7"
  ClassDef(Scene, 1); // Representation of a Scene with global lamps and ability do define GL state
}; // endclass Scene


} // endnamespace gled

#endif
