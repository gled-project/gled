// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

//__________________________________________________________________________
// ZGlPerspective
//
// Sets-up GL projection and modelview matrices for orthographic
// viewing in fixed or pixel-based coordinates.
//
// Negative offsets are handled specially in OrthoPixel mode - they are
// interpreted as distance from right / upper edge.
//
// Mode OrthoTrueAspect takes the smaller of actual w/h as a unit and
// creates a (-1,1) view with center at the screen center.
// For now mOrthoW/H and mOx/y/z are ignored.

#include "ZGlPerspective.h"

using namespace gled;

#include "ZGlPerspective.c7"

/**************************************************************************/

void ZGlPerspective::_init()
{
  mViewMode  = VM_Nop;
  mOrthoW    = 10; mOrthoH   = 10;
  mOrthoNear =  0; mOrthoFar = 1;
  mOx = mOy = mOz = 0;
}

/**************************************************************************/

void ZGlPerspective::StandardPersp()
{
  ZGlPerspective& X = *this;
  X.SetViewMode(VM_Nop);
  X.SetOrthoW(10.000000); X.SetOrthoH(10.000000);
  X.SetOrthoNear(0); X.SetOrthoFar(1);
}

void ZGlPerspective::StandardFixed()
{
  ZGlPerspective& X = *this;
  X.SetViewMode(VM_OrthoFixed);
  X.SetOrthoW(10.000000); X.SetOrthoH(10.000000);
  X.SetOrthoNear(0); X.SetOrthoFar(1);
}

void ZGlPerspective::StandardTrueAspect()
{
  ZGlPerspective& X = *this;
  X.SetViewMode(VM_OrthoTrueAspect);
  X.SetOrthoNear(0); X.SetOrthoFar(1);
}

void ZGlPerspective::StandardPixel()
{
  ZGlPerspective& X = *this;
  X.SetViewMode(VM_OrthoPixel);
  X.SetOrthoW(10.000000); X.SetOrthoH(10.000000);
  X.SetOrthoNear(0); X.SetOrthoFar(1);
}

/**************************************************************************/
