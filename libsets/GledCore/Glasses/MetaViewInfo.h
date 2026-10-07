// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef GledCore_MetaViewInfo_H
#define GledCore_MetaViewInfo_H

#include <Glasses/ZList.h>

namespace gled {

class MetaViewInfo : public ZList {
  MAC_RNR_FRIENDS(MetaViewInfo);

private:
  void _init();

protected:
  Int_t		mW;        // X{GS} 7 Value(-range=>[1,256,1], -join=>1)
  Int_t		mH;        // X{GS} 7 Value(-range=>[1,256,1])

  Bool_t	bExpertP;  // X{GS} 7 Bool()

public:
  MetaViewInfo(const Text_t* n="MetaViewInfo", const Text_t* t=0) :
    ZList(n,t) { _init(); }

  void Size(int w, int h);  // X{E}

#include "MetaViewInfo.h7"
  ClassDef(MetaViewInfo, 1);
}; // endclass MetaViewInfo


} // endnamespace gled

#endif
