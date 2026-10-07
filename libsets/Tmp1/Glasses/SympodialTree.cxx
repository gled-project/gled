// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "SympodialTree.h"
using namespace gled;
#include "SympodialTree.c7"

// SympodialTree

//______________________________________________________________________________
//
//

//==============================================================================


SympodialTree::SympodialTree(const Text_t* n, const Text_t* t) :
  MonopodialTree(n, t)
{
 // _init();
}

SympodialTree::~SympodialTree()
{}

//==============================================================================

void SympodialTree::ExpandRule(const Text_t* rule, TwoParam& parent, ParametricSystem::Segments_t& newExp)
{
  const static float widthDecrease = TMath::Sqrt(0.5f);
  bool firstStack = true;
  for (size_t k = 0; k < strlen(rule); ++k)
  {
    newExp.push_back(TwoParam(rule[k], parent.mParam1, parent.mParam2));  

    TwoParam& s = newExp.back();
    if (s.mType == ']')
    {
      firstStack = false;
    }
    
    if (s.mType == '&' || s.mType == '^')
    {
      s.mParam1 = firstStack ? mTrunkAngle : mLateralAngle;
    }
    else if (s.mType == '+' || s.mType == '-')
    {
      s.mParam1 = firstStack ? mTrunkAngle : mLateralAngle;
    }
    else if (s.mType == '\\' || s.mType == '/')
    {
      s.mParam1 = 180;
    }
    else if (s.mType == 'A' || s.mType == 'B')
    {
      float contraction = firstStack ? mTrunkContraction : mLateralContraction;
      s.mParam1 *= contraction;
      s.mParam2 *= widthDecrease;
    }
  } 
  
}