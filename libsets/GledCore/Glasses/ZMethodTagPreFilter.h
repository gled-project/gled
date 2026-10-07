// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef GledCore_ZMethodTagPreFilter_H
#define GledCore_ZMethodTagPreFilter_H

#include <Glasses/ZMirFilter.h>

namespace gled {

class ZMethodTagPreFilter : public ZMirFilter {
  MAC_RNR_FRIENDS(ZMethodTagPreFilter);

private:
  void _init();

protected:
  TString	mTags;		// X{GS} 7 Textor(-width=>20)
  ZLink<ZMirFilter>	mFilter;	// X{gS} L{}

public:
  ZMethodTagPreFilter(const Text_t* n="ZMethodTagPreFilter", const Text_t* t=0) :
    ZMirFilter(n,t) { _init(); }

  virtual Result_e FilterMIR(ZMIR& mir);

#include "ZMethodTagPreFilter.h7"
  ClassDef(ZMethodTagPreFilter, 1);
}; // endclass ZMethodTagPreFilter


} // endnamespace gled

#endif
