// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "STabletPoint.h"

using namespace gled;

// STabletPoint

//______________________________________________________________________________
//
//

//==============================================================================

void STabletPoint::Print() const
{
  printf("%8.3lf %8.3lf %8.3lf, %8.3lf, %8.3lf\n", x, y, z, t, p); 
}
