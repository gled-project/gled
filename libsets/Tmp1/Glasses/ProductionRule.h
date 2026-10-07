// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef Tmp1_ProductionRule_H
#define Tmp1_ProductionRule_H

#include <Glasses/ZGlass.h>
#include <Glasses/ZNode.h>

namespace gled {

class ProductionRule : public ZGlass
{
  MAC_RNR_FRIENDS(ProductionRule);

private:
  void _init();

protected:
  ZNode*  mConsumer; 
  TString	mRule;		//  X{RGE} 7 Textor()

public:
  ProductionRule(const Text_t* n="ProductionRule", const Text_t* t=0);
  virtual ~ProductionRule();
  
  void SetRule(const Text_t* t);
  
  void SetConsumer(ZNode* n) {mConsumer = n;}
#include "ProductionRule.h7"
  ClassDef(ProductionRule, 1);
}; // endclass ProductionRule

} // endnamespace gled

#endif
