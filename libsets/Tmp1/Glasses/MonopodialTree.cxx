// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "MonopodialTree.h"
using namespace gled;
#include "MonopodialTree.c7"
#include "TMath.h"

// MonopodialTree

//______________________________________________________________________________
//
//

void MonopodialTree::_init()
{}

MonopodialTree::MonopodialTree(const Text_t* n, const Text_t* t) :
ParametricSystem(n, t),
mTrunkContraction(0.9),
mLateralContraction(0.6),
mTrunkAngle(45),
mLateralAngle(45),
mStartLength(1),
mStartWidth(0.1)
{
  _init();
  mStart = "A";
}

MonopodialTree::~MonopodialTree()
{}

void
MonopodialTree::InitialiseExpression()
{
  assert(mExpression.empty());
  mExpression.push_back(TwoParam('A', mStartLength, mStartWidth));
}

void
MonopodialTree::SetStartWidth(float x)
{
  mStartWidth = x;
  mStampReqTring = Stamp(FID());
}

void
MonopodialTree::SetStartLength(float x)
{
  mStartLength = x;
  mStampReqTring = Stamp(FID());
}

void
MonopodialTree::ExpandRule(const Text_t* rule, TwoParam& parent, ParametricSystem::Segments_t& newExp )
{
  const static float widthDecrease = TMath::Sqrt(0.5f);
  static float divergenceAngle = 137;
  
  bool firstStep = true;
  
  for (size_t k = 0; k < strlen(rule); ++k)
  {
    newExp.push_back(TwoParam(rule[k], parent.mParam1, parent.mParam2));  
    
    TwoParam& s = newExp.back();
    if (s.mType == ']')
      firstStep = false;
    
    if (s.mType == '&' || s.mType == '^')
    {
      s.mParam1 = mTrunkAngle;
    }
    else if (s.mType == '+' || s.mType == '-')
    {
      s.mParam1 = mLateralAngle;
    }
    else if (s.mType == '\\' || s.mType == '/')
    {
      s.mParam1 = divergenceAngle;
    }
    else if (s.mType == 'A' || s.mType == 'B' || s.mType == 'C')
    {
      float contraction = (firstStep) ? mLateralContraction : mTrunkContraction ;
      //   printf("contractin %f \n", contraction);
      s.mParam1 *= contraction;
      s.mParam2 *= widthDecrease;
    }
  }
}

