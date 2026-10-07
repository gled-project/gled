// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

//__________________________________________________________________________
// MetaWeedInfo
//
//

#include "MetaWeedInfo.h"

using namespace gled;

#include "MetaWeedInfo.c7"

/**************************************************************************/

void MetaWeedInfo::_init()
{
  // *** Set all links to 0 ***
  mX = 0; mY = 0; mW = 12; mH = 1;

  bColorP  = false;  mColor.rgba(0.75, 0.75, 0.75);
  bLabelP  = false;
  bAlignP  = false; bAInside = false; mALtRt  = mAUpDn = 0;
  mBoxType = BT_None; bBDown = false; bBFrame = false;
}

/**************************************************************************/

void MetaWeedInfo::Resize(int x, int y, int w, int h)
{
  mX = x; mY = y; mW = w; mH = h;
  Stamp(FID());
}

void MetaWeedInfo::Color(Float_t r, Float_t g, Float_t b)
{
  bColorP = true;
  SetColor(r, g, b);
}

void MetaWeedInfo::Label(const Text_t* l)
{
  bLabelP = true;
  SetTitle(l);
  Stamp(FID());
}

void MetaWeedInfo::Align(Bool_t inside, Char_t lr, Char_t ud)
{
  bAlignP  = true;
  bAInside = inside;
  mALtRt = lr; mAUpDn = ud;
  Stamp(FID());
}

void MetaWeedInfo::Box(BoxType_e box, Bool_t downp, Bool_t framep)
{
  mBoxType = box;
  bBDown = downp; bBFrame = framep;
  Stamp(FID());
}

/**************************************************************************/
