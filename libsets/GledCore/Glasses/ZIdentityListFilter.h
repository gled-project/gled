// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef GledCore_ZIdentityListFilter_H
#define GledCore_ZIdentityListFilter_H

#include <Glasses/ZMirFilter.h>

namespace gled {
class ZHashList;

class ZIdentityListFilter : public ZMirFilter
{
  MAC_RNR_FRIENDS(ZIdentityListFilter);

private:
  void _init();

protected:
  ZLink<ZHashList> mIdentities; // X{gS} L{}
  UChar_t          mOnMatch;    // X{gS} 7 PhonyEnum(-type=>ZMirFilter::Result_e, -names=>[R_Allow,R_Deny],-width=>6)

public:
  ZIdentityListFilter(const Text_t* n="ZIdentityListFilter", const Text_t* t=0) :
    ZMirFilter(n,t) { _init(); }

  virtual Result_e FilterMIR(ZMIR& mir);

#include "ZIdentityListFilter.h7"
  ClassDef(ZIdentityListFilter, 1);
}; // endclass ZIdentityListFilter


} // endnamespace gled

#endif
