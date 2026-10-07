// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "XTReqCanvas.h"

#include "TCanvas.h"
#include "TSystem.h"
#include "TClass.h"

using namespace gled;

//==============================================================================
// XTReqCanvas
//------------------------------------------------------------------------------
// Create canvas in cross-thread request to ROOT thread.
//------------------------------------------------------------------------------

void XTReqCanvas::Act()
{
  fCanvas = new TCanvas(fName, fTitle, fW, fH);
  if (fCanvas && (fNPx > 1 || fNPy > 1))
  {
    fCanvas->Divide(fNPx, fNPy);
    fCanvas->Update();
  }

  // Make sure window is on screen before we pass it back ...
  gSystem->ProcessEvents();
}

TCanvas* XTReqCanvas::Request(const char* name, const char* title,
			      int w, int h, int npx, int npy)
{
  std::unique_ptr<XTReqCanvas> creq(new XTReqCanvas(name, title, w, h, npx, npy));
  creq->ShootRequestAndWait();
  return creq->fCanvas;
}


//==============================================================================
// XTReqPadUpdate
//------------------------------------------------------------------------------
// Create canvas in cross-thread request to ROOT thread.
//------------------------------------------------------------------------------

void XTReqPadUpdate::Act()
{
  fPad->Modified();
  fPad->Update();
}

void XTReqPadUpdate::Update(TVirtualPad* p)
{
  (new XTReqPadUpdate(p))->ShootRequest();
}
