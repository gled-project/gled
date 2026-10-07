// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

//__________________________________________________________________________
// WGlWidget
//
//

#include "WGlWidget.h"

using namespace gled;

#include "WGlWidget.c7"

/**************************************************************************/

void WGlWidget::_init()
{
  mDx = mDy = 0;
}

/**************************************************************************/

void WGlWidget::SetDaughterCbackAlpha(ZGlass* lens, Int_t recurse_lvl)
{
  // No locking for list, daughters done.

  Stepper<WGlWidget> stepper(this);
  while (stepper.step()) {
    stepper->SetCbackAlpha(lens);
    if (recurse_lvl != 0)
      stepper->SetDaughterCbackAlpha(lens, recurse_lvl - 1);
  }
}

void WGlWidget::SetDaughterCbackStuff(ZGlass* lens, Int_t recurse_lvl)
{
  // Set various members of daughter widgets depending on the contents
  // of their title:
  // 'LensName'  - set name
  // 'LensAlpha' - set callback alpha
  // 'LensBeta'  - set callback beta
  // Several tokens can be present, all will be processed.
  //
  // No locking for list, daughters done.

  Stepper<WGlWidget> stepper(this);
  while (stepper.step())
  {
    if (stepper->RefTitle().Contains("LensName"))
    {
      stepper->SetName(lens->GetName());
    }
    if (stepper->RefTitle().Contains("LensAlpha"))
    {
      stepper->SetCbackAlpha(lens);
    }
    if (stepper->RefTitle().Contains("LensBeta"))
    {
      stepper->SetCbackBeta(lens);
    }

    if (recurse_lvl != 0)
      stepper->SetDaughterCbackStuff(lens, recurse_lvl - 1);
  }
}

/**************************************************************************/
