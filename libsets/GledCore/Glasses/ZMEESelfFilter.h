// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef GledCore_ZMEESelfFilter_H
#define GledCore_ZMEESelfFilter_H

#include <Glasses/ZMirFilter.h>

namespace gled {

class ZMEESelfFilter : public ZMirFilter
{
  MAC_RNR_FRIENDS(ZMEESelfFilter);

private:
  void _init();

protected:

public:
  ZMEESelfFilter(const Text_t* n="ZMEESelfFilter", const Text_t* t=0) :
    ZMirFilter(n,t) { _init(); }

  virtual Result_e FilterMIR(ZMIR& mir);

#include "ZMEESelfFilter.h7"
  ClassDef(ZMEESelfFilter, 1);
}; // endclass ZMEESelfFilter


} // endnamespace gled

#endif
