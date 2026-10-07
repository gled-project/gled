// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include <Glasses/Camera.h>

using namespace gled;

#include "Camera.c7"

//==============================================================================

void Camera::Home()
{
  mTrans = mHomeTrans;
  Stamp(FID());
}

void Camera::Identity()
{
  mTrans.UnitTrans();
  Stamp(FID());
}

//==============================================================================

void Camera::SetHomeTrans()
{
  mHomeTrans = mTrans;
}

void Camera::ResetHomeTrans()
{
  mHomeTrans.UnitTrans();
}
