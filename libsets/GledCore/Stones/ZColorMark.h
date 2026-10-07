// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef GledCore_ZColorMark_H
#define GledCore_ZColorMark_H

#include <Stones/ZColor.h>

namespace gled {

class ZColorMark : public ZColor
{
protected:
  Float_t	mMark;

public:
  ZColorMark() : ZColor(), mMark(0) {}
  ZColorMark(Float_t m, const ZColor& c) : ZColor(c), mMark(m) {}
  ZColorMark(Float_t m, Float_t r, Float_t g, Float_t b, Float_t a = 1) :
    ZColor(r,g,b,a), mMark(m) {}

  Float_t m() const { return mMark; }
  void m(Float_t m) { mMark = m; }

  Float_t mark() const { return mMark; }
  void mark(Float_t m) { mMark = m; }

#include "ZColorMark.h7"
  ClassDefNV(ZColorMark, 1); // Color with additional floating-point mark.
}; // endclass ZColorMark

} // endnamespace gled

#endif
