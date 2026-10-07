// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef GledCore_MetaSubViewInfo_H
#define GledCore_MetaSubViewInfo_H

#include <Glasses/ZList.h>

namespace gled {

class MetaSubViewInfo : public ZList {
  MAC_RNR_FRIENDS(MetaSubViewInfo);

private:
  void _init();

protected:
  Int_t		mX;      // X{GS} 7 Value(-range=>[0,256,1], -join=>1)
  Int_t		mY;      // X{GS} 7 Value(-range=>[0,256,1])

public:
  MetaSubViewInfo(const Text_t* n="MetaSubViewInfo", const Text_t* t=0) :
    ZList(n,t) { _init(); }

  void Position(int x, int y);  // X{E}

#include "MetaSubViewInfo.h7"
  ClassDef(MetaSubViewInfo, 1);
}; // endclass MetaSubViewInfo


} // endnamespace gled

#endif
