// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ZColor.h"

using namespace gled;


//______________________________________________________________________________
//
// Color class using RGBA float quadruple.

std::ostream& operator<<(std::ostream& s, const ZColor& c)
{
  return s << c[0] <<","<< c[1] <<","<< c[2] <<","<< c[3];
}
