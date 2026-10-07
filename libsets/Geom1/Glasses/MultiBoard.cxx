// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

//__________________________________________________________________________
// MultiBoard
//
//

#include "MultiBoard.h"
#include <Glasses/ZHashList.h>

#include <Glasses/ZImage.h>
#include <Glasses/ZQueen.h>

using namespace gled;

#include "MultiBoard.c7"

/**************************************************************************/

void MultiBoard::_init()
{}

void MultiBoard::AdEnlightenment()
{
  PARENT_GLASS::AdEnlightenment();
  if(mSlides == 0) {
    assign_link<ZHashList>(mSlides, FID(), "Slides", GForm("Slides of MultiBoard %s", GetName()));
    mSlides->SetElementFID(ZImage::FID());
  }
}

/**************************************************************************/

void MultiBoard::First()
{
  ZImage* i = dynamic_cast<ZImage*>(mSlides->FrontElement());
  if(i == 0) return;
  SetTexture(i);
}

void MultiBoard::Last()
{
  ZImage* i = dynamic_cast<ZImage*>(mSlides->BackElement());
  if(i == 0) return;
  SetTexture(i);
}

void MultiBoard::Prev()
{
  ZImage* i = dynamic_cast<ZImage*>(mSlides->ElementBefore(mTexture.get()));
  if(i == 0) First();
  else	     SetTexture(i);
}

void MultiBoard::Next()
{
  ZImage* i = dynamic_cast<ZImage*>(mSlides->ElementAfter(mTexture.get()));
  if(i == 0) Last();
  else	     SetTexture(i);
}

