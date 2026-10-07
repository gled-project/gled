// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef @LIBSET@_@CLASS@_H
#define @LIBSET@_@CLASS@_H

@BASE_INCLUDE@

namespace gled {

class @CLASS@@BASE_CLAUSE@
{
private:
  void _init();

protected:

public:
  @CLASS@();
  virtual ~@CLASS@();

@P7_INCLUDE@  ClassDef(@CLASS@, 1);
}; // endclass @CLASS@

} // endnamespace gled

#endif
