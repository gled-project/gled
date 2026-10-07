// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef Tmp1_TernaryTree_H
#define Tmp1_TernaryTree_H

#include <Glasses/MonopodialTree.h>
//#include <Glasses/ZPoint.h>

namespace gled {

class TernaryTree : public MonopodialTree
{
  MAC_RNR_FRIENDS(TernaryTree);
  
private:
  void _init();
  
protected:
  virtual void ExpandRule(const Text_t* rule, TwoParam& parent, ParametricSystem::Segments_t& out);
  virtual void ExpandExpression(Segments_t& oldExp, Segments_t& newExp, int level);
  virtual void InitialiseExpression();
  
  float  mDivergenceAngle1;          // X{GST} 7 Value(-range=>[0, 360, 1])
  float  mDivergenceAngle2;          // X{GST} 7 Value(-range=>[0, 360, 1])
  float  mSusceptibility;            // X{GST} 7 Value(-range=>[0, 1, 1, 100])
  ZPoint mTorsion;                   // X{GETR} 7 ZPoint()  
  ZPoint mTorsionNormalized;
    
    
public:
  TernaryTree(const Text_t* n="TernaryTree", const Text_t* t=0);
  virtual ~TernaryTree();
  
  void SetTorsion(Double_t x, Double_t y, Double_t z);
  void   ResetTorsion();    // X{ED}  7 MButt()
  
  const static float s_Width_Increase;
#include "TernaryTree.h7"
  ClassDef(TernaryTree, 1);
}; // endclass TernaryTree

} // endnamespace gled

#endif
