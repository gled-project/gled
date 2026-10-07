// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef GledCore_ZMirFilter_H
#define GledCore_ZMirFilter_H

#include <Glasses/ZGlass.h>

namespace gled {

class ZMirFilter : public ZGlass
{
  MAC_RNR_FRIENDS(ZMirFilter);

public:
  enum Result_e { R_None=0x1, R_Allow=0x2, R_Deny=0x4 };

private:
  void _init();

protected:
  UChar_t	mMapNoneTo;	// X{GS} 7 PhonyEnum(-type=>Result_e, -width=>6)

public:
  ZMirFilter(const Text_t* n="ZMirFilter", const Text_t* t=0) : ZGlass(n,t)
  { _init(); }

  virtual Result_e FilterMIR(ZMIR& mir);

  Result_e FinaliseResult(Result_e r);

  static Result_e NegateResult(Result_e r);

#include "ZMirFilter.h7"
  ClassDef(ZMirFilter, 1);
}; // endclass ZMirFilter


/**************************************************************************/
// Inlines
/**************************************************************************/

inline
ZMirFilter::Result_e ZMirFilter::FilterMIR(ZMIR& mir)
{
  return (Result_e)mMapNoneTo;
}

/**************************************************************************/

inline
ZMirFilter::Result_e ZMirFilter::FinaliseResult(ZMirFilter::Result_e r)
{
  return (r == R_None) ? (Result_e)mMapNoneTo : r;
}

inline
ZMirFilter::Result_e ZMirFilter::NegateResult(ZMirFilter::Result_e r)
{
  if(r == R_Allow) 	return R_Deny;
  else if(r == R_Deny)	return R_Allow;
  else			return r;
}

} // endnamespace gled

#endif
