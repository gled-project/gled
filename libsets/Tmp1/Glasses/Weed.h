// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef Tmp1_Weed_H
#define Tmp1_Weed_H

#include <Glasses/ZNode.h>
#include <Glasses/ZVector.h>
#include <Stones/ZColor.h>

/**************************************************************************/
// Weed - a class the serves as start in introduction to plant modeling.  //
// Covers the first chapter of book The Algoritmic Beauty Of Plants by
// Przemwsylaw Pruskinewitch and Arstid Lindenmayer.
/**************************************************************************/

namespace gled {

class Weed : public ZNode
{
  MAC_RNR_FRIENDS(Weed);

private:
  typedef std::map<char, int>              NameMap_t;
  typedef std::map<char, int>::iterator    NameMap_i;  
  NameMap_t    mNameMap;
  char*        mExpression;
  int          mLength;
  
  
  void _init();
  char* ExpandString(char* oldExp, int oldSize, char* newExp, int level);

protected:
  Int_t    mLevel;            // X{GST} 7 Value(-range=>[1, 7, 1])
  Double_t mAngle;            // X{GST} 7 Value(-range=>[0, 100, 1, 180])
  TString  mStart;            // X{GST} 7 Textor()  
  ZLink<ZVector> mRules;      // X{GS}  L {}
  
  ZColor	 mLineColor;        // X{GSP} 7 ColorButt()
  ZColor	 mLeafColor;        // X{GSP} 7 ColorButt()
  ZColor	 mFlowerColor;        // X{GSP} 7 ColorButt()

public:
  Weed(const Text_t* n="Weed", const Text_t* t=0);
  virtual ~Weed();
  
  virtual void Produce();
  const char* GetExpression() const { return mExpression; }
  int GetExpressionLength()   const { return mLength; }
  
  void DumpInfo() const;    // X{ED}  7 MButt()
  
#include "Weed.h7"
  ClassDef(Weed, 1);
}; // endclass Weed

} // endnamespace gled

#endif
