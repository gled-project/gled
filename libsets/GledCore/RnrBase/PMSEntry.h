// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef GledCore_PMSEntry_H
#define GledCore_PMSEntry_H

#include <Stones/ZTrans.h>

namespace gled {
class ZNode;

struct PMSEntry
{
  PMSEntry   *fPrev;
  ZNode      *fNode;

  ZTrans      fLocal;
  ZTrans     *fToGCS;
  ZTrans     *fFromGCS;
  Bool_t      bTo, bFrom;

  PMSEntry() : fPrev(0), fNode(0),
	       fToGCS(0), fFromGCS(0), bTo(0), bFrom(0)
  {}

  ~PMSEntry()
  { delete fToGCS; delete fFromGCS; }

  ZTrans& ToGCS();
  ZTrans& FromGCS();
};

} // endnamespace gled

#endif
