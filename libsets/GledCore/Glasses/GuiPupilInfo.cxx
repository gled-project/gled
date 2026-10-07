// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

//__________________________________________________________________________
// GuiPupilInfo
//
//

#include "GuiPupilInfo.h"
#include <Glasses/ZQueen.h>

using namespace gled;

#include "GuiPupilInfo.c7"

/**************************************************************************/

void GuiPupilInfo::_init()
{
  // Override from SubShellInfo:
  mCtorLibset = "GledCore";
  mCtorName   = "GuiPupil";

  mPupil   = 0;
  mCameras = 0;
}

/**************************************************************************/

void GuiPupilInfo::AssertDefaultPupil()
{
  if(mPupil == 0) {
    PupilInfo* p = new PupilInfo("Swallowed Pupil", GForm("Pupil of %s", GetName()));
    mQueen->CheckIn(p);
    SetPupil(p);
  }
}

/**************************************************************************/
