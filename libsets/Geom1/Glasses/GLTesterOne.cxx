// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

//__________________________________________________________________________
// GLTesterOne
//
// Speed tests for GL operations. User can specify the number of outer and
// inner loops; the outer-loop timings are histogrammed with micro-second
// precision.
//
// Test(s) performed depend on value of mTestSelection variable:
//
// 1) TT_RnrAtom - point/line triangle rendering speed. Different modes
//    allow different ways of setting up the transformation matrix.
// 2) MatrixOps  - speed of retrieving matrices from GL.
//
// See also code of GLTesterOne_GL_Rnr.

#include "GLTesterOne.h"

#include <Gled/GTime.h>
#include <Gled/TRootXTReq.h>

#include <TCanvas.h>
#include <TMath.h>
#include <TROOT.h>

using namespace gled;

#include "GLTesterOne.c7"

/**************************************************************************/

void GLTesterOne::_init()
{
  mPupil = 0;

  mTestSelection = TT_RnrAtom;

  mRAtom  = RA_Point;
  mTMode  = TM_Vertex;
  mNSteps = 1000;
  mXMax   = 100;
  mTringU = 0.2;
  mTringV = 0.1;

  mMatrixOps = MO_GetFloat;

  bUseDispList = false;
  bPrint       = true;

  mNRedraws = 1000;
  mNTrial   = 20;

  h1TStat = new TH1F("TStat", "GlOne Time Statistics", 10, 0, 1);
}

/**************************************************************************/

void GLTesterOne::register_result(Double_t t)
{
  if(h1TStat != 0) {
    h1TStat->Fill(t);
  }
}

void GLTesterOne::RunTest()
{
  h1TStat->Reset();
  h1TStat->SetBins(200, 0, 1);

  for(int i=0; i<mNTrial; ++i) {
    mPupil->Redraw();
  }
  while(h1TStat->GetEntries() < mNTrial) GTime::SleepMiliSec(10);

  Double_t m = h1TStat->GetMean(), r = h1TStat->GetRMS();
  Double_t min = TMath::Max(m - 5*r, 0.0);
  Double_t max = m + 5*r;
  Int_t nbins = TMath::Min(200, Int_t((max - min)/1e-6)); // Have mu-sec precision.

  h1TStat->Reset();
  h1TStat->SetBins(nbins, min, max);

  for(int i=0; i<mNRedraws; ++i) {
    mPupil->Redraw();
  }
  while(h1TStat->GetEntries() < mNRedraws) GTime::SleepMiliSec(10);

  // The ROOT thread draws a copy of the histogram into the canvas "GlOne".
  // Clearing the canvas deletes the previous copy.
  TH1 *h = (TH1*) h1TStat->Clone("TStatShow");
  h->SetDirectory(0);
  h->SetBit(kCanDelete);
  TRootXTReq::Post([h]()
  {
    TCanvas *c = (TCanvas*) gROOT->GetListOfCanvases()->FindObject("GlOne");
    if (c == 0)
      c = new TCanvas("GlOne", "GlOne Time Statistics");
    c->cd();
    c->Clear();
    h->Draw();
    c->Modified();
    c->Update();
  });
}

/**************************************************************************/
