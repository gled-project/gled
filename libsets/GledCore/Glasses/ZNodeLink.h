// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef GledCore_ZNodeLink_H
#define GledCore_ZNodeLink_H

#include <Glasses/ZNode.h>

namespace gled {

class ZNodeLink : public ZNode {
  // 7777 RnrCtrl(RnrBits(2,0,6,0, 0,0,0,5))
  MAC_RNR_FRIENDS(ZNodeLink);
private:
  void		_init() 	{ mLens = 0; }

protected:
  ZLink<ZGlass>	mLens;		// X{gS} L{} RnrBits{0,4,0,0, 0,0,0,5}

public:
  ZNodeLink(const Text_t* n="ZNodeLink", const Text_t* t=0) : ZNode(n,t)
  { _init(); }

#include "ZNodeLink.h7"
  ClassDef(ZNodeLink, 1);
}; // endclass ZNodeLink


} // endnamespace gled

#endif
