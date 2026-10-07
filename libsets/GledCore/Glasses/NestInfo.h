// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef GledCore_NestInfo_H
#define GledCore_NestInfo_H

#include <Glasses/SubShellInfo.h>
#include <Eye/Ray.h>

namespace gled {

class NestInfo : public SubShellInfo {
  MAC_RNR_FRIENDS(NestInfo);

public:
  enum LeafLayout_e {
    LL_Ants,
    LL_Custom
  };

  enum PrivRayQN_e  { PRQN_offset = RayNS::RQN_user_0,
		      PRQN_layout_change,
		      PRQN_rewidth
  };

private:
  void _init();

protected:
  // Creation parameters.
  Bool_t	bShowSelf;      // X{GS} 7 Bool(-join=>1)
  UInt_t	mMaxChildExp;	// X{GS} 7 Value(-range=>[0,1000,1])

  // Widths of major elements.
  Int_t		mWName;   // X{GS} Ray{Rewidth} 7 Value(-range=>[18,256,1], -join=>1)
  Int_t		mWAnt;    // X{GS} Ray{Rewidth} 7 Value(-range=>[0,40,1])
  Int_t		mWIndent; // X{GS} Ray{Rewidth} 7 Value(-range=>[0,10,1], -join=>1)
  Int_t		mWSepBox; // X{GS} Ray{Rewidth} 7 Value(-range=>[0,5,1])

  // Layout specification.
  ZLink<ZList>	mLayoutList;	// X{GS} L{}
  LeafLayout_e  mLeafLayout;    // X{GS} Ray{Layout} 7 PhonyEnum()
  TString       mLayout;        // X{GS} Ray{Layout} 7 Textor()

public:
  NestInfo(const Text_t* n="NestInfo", const Text_t* t=0) :
    SubShellInfo(n,t) { _init(); }

  void ImportLayout(ZGlass* src); // X{E} C{1} 7 MCWButt()

  void ImportKings(); // X{E} 7 MButt()

  void EmitLayoutRay();  // X{E} 7 MButt(-join=>1)
  void EmitRewidthRay(); // X{E} 7 MButt()

  static const Text_t* sLayoutPath;

#include "NestInfo.h7"
  ClassDef(NestInfo, 1); // Glass representation of GUI browser 'FTW_Nest'.
}; // endclass NestInfo


} // endnamespace gled

#endif
