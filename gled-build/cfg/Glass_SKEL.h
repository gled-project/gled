// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef @LIBSET@_@CLASS@_H
#define @LIBSET@_@CLASS@_H

#include <Glasses/@BASE@.h>

namespace gled {

class @CLASS@ : public @BASE@
{
  MAC_RNR_FRIENDS(@CLASS@);

private:
  void _init();

protected:

public:
  @CLASS@(const Text_t* n="@CLASS@", const Text_t* t=0);
  virtual ~@CLASS@();

#include "@CLASS@.h7"
  ClassDef(@CLASS@, 1);
}; // endclass @CLASS@

} // endnamespace gled

#endif
