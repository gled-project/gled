// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

//__________________________________________________________________________
// CameraInfo
//
// Copy of PupilInfo's camera information.

// Should be solved in some other way ... say via p7 supporting inclusion of
// stone or non-glass-base members together with widget specification.

#include "CameraInfo.h"

using namespace gled;

#include "CameraInfo.c7"

/**************************************************************************/

void CameraInfo::_init()
{
  bFixCameraBase = bFixLookAt = bFixUpReference = true;


  mCameraBase    = 0;

  mLookAt        = 0;
  mLookAtMinDist = 0.1;

  mUpReference   = 0;
  mUpRefAxis     = 3;
  bUpRefLockDir  = true;
  mUpRefMinAngle = 10;

  mProjMode = P_Perspective;
  mZFov     = 90;   mZSize    = 20;
  mYFac     = 1;    mXDist    = 10;
  mNearClip = 0.01; mFarClip  = 100;
  mDefZFov  = 90;   mDefZSize = 20;
}

/**************************************************************************/

void CameraInfo::SetupZFov(Float_t zfov)
{
  SetZFov(zfov);
  SetDefZFov(zfov);
}

void CameraInfo::SetupZSize(Float_t zsize)
{
  SetZSize(zsize);
  SetDefZSize(zsize);
}

