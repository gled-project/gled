// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

//__________________________________________________________________________
// TimeScreenText
//
//

#include "TimeScreenText.h"

using namespace gled;

#include "TimeScreenText.c7"

/**************************************************************************/

void TimeScreenText::_init()
{}

/**************************************************************************/

void TimeScreenText::TimeTick(Double_t t, Double_t dt)
{
  SetText(GForm(mFormat, t));
}

/**************************************************************************/
