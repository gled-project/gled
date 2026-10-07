// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef Tmp1_SympodialTree_H
#define Tmp1_SympodialTree_H

#include <Glasses/MonopodialTree.h>

namespace gled {

class SympodialTree : public MonopodialTree
{
  MAC_RNR_FRIENDS(SympodialTree);

private:
  
protected:
  
  virtual void ExpandRule(const Text_t* rule, TwoParam& parent, ParametricSystem::Segments_t& out);
  
public:
  SympodialTree(const Text_t* n="SympodialTree", const Text_t* t=0);
  virtual ~SympodialTree();

#include "SympodialTree.h7"
  ClassDef(SympodialTree, 1);
}; // endclass SympodialTree

} // endnamespace gled

#endif
