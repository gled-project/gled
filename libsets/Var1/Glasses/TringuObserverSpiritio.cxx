// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "TringuObserverSpiritio.h"
#include <Glasses/ZNode.h>
#include "TringuCam.h"

#include <Glasses/Camera.h>
#include <Glasses/ZQueen.h>

using namespace gled;

#include "TringuObserverSpiritio.c7"

// TringuObserverSpiritio

//______________________________________________________________________________
//
// Right now ... just forward all to TringuCam, also in GL-Rnr.
// Some functionality of TringuCam will come here.

//==============================================================================

void TringuObserverSpiritio::_init()
{}

TringuObserverSpiritio::TringuObserverSpiritio(const Text_t* n, const Text_t* t) :
  Spiritio(n, t)
{
  _init();
}

TringuObserverSpiritio::~TringuObserverSpiritio()
{}

//==============================================================================

void TringuObserverSpiritio::AdEnlightenment()
{
  // Create the camera.

  PARENT_GLASS::AdEnlightenment();

  if (mCamera == 0)
  {
    assign_link<Camera>(mCamera, FID(), "TringuObserverCamera",
                        "Camera of TringuObserverSpiritio");
    mCamera->SetMIRActive(false);
  }
}

//==============================================================================

void TringuObserverSpiritio::Activate()
{
  mCamera->SetParent(*mTringuCam);
  // mCamera->Home();

  PARENT_GLASS::Activate();
}

void TringuObserverSpiritio::Deactivate()
{
  PARENT_GLASS::Deactivate();

  mCamera->SetParent(0);
}

//==============================================================================

void TringuObserverSpiritio::TimeTick(Double_t t, Double_t dt)
{
  mTringuCam->TimeTick(t, dt);
}
