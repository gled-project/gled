// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef Gled_ZSubTree_H
#define Gled_ZSubTree_H

#include <Glasses/ZGlass.h>

namespace gled {

class ZSubTree : public ZGlass {
  MAC_RNR_FRIENDS(ZSubTree);
private:
  void _init();

protected:
  ZLink<ZGlass>	mRoot;		// X{gS} L{}

  Int_t		mDepth;		// X{gS} 7 Value(-range=>[-1,100,1,1])
  Bool_t	bFollowLinks;	// X{gS} 7 Bool()
  Bool_t	bFollowLists;	// X{gS} 7 Bool()

public:
  ZSubTree(const Text_t* n="ZSubTree", const Text_t* t=0) : ZGlass(n,t)
  { _init(); }

  ZSubTree(Int_t d, Bool_t flnk, Bool_t flst,
	   const Text_t* n="ZSubTree", const Text_t* t=0) :
    ZGlass(n,t),
    mDepth(d), bFollowLinks(flnk), bFollowLists(flst)
  {}

#include "ZSubTree.h7"
  ClassDef(ZSubTree, 1);
}; // endclass ZSubTree


} // endnamespace gled

#endif
