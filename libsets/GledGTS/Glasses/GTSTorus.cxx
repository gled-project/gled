// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "GTSTorus.h"

#include <Glasses/GTSIsoMaker.h>

using namespace gled;

#include "GTSTorus.c7"

// GTSTorus

//______________________________________________________________________________
//
//

//==============================================================================

void GTSTorus::_init()
{
  mRM = 1;
  mRm = 0.5;
  mStep = 0.01;
}

GTSTorus::GTSTorus(const Text_t* n, const Text_t* t) :
  GTSurf(n, t)
{
  _init();
}

GTSTorus::~GTSTorus()
{}

//==============================================================================

void GTSTorus::GTSIsoBegin(GTSIsoMaker* maker, Double_t /*iso_value*/)
{
  mRminvsqr = 1.0 / (mRm * mRm);

  Double_t RR = 1.05 * (mRM + mRm);
  Double_t rr = 1.05 * mRm;

  GLensReadHolder _lck(maker);
  maker->SetValue(1);
  maker->SetXAxis(-RR, RR, TMath::Nint(2.0*RR/mStep));
  maker->SetYAxis(-RR, RR, TMath::Nint(2.0*RR/mStep));
  maker->SetZAxis(-rr, rr, TMath::Nint(2.0*rr/mStep));
  maker->SetInvertCartesian(false);
  maker->SetInvertTetra(true);
}

Double_t GTSTorus::GTSIsoFunc(Double_t x, Double_t y, Double_t z)
{
  const Double_t r = TMath::Sqrt(x*x + y*y) - mRM;
  return mRminvsqr * (r*r + z*z);
}

Double_t GTSTorus::GTSIsoGradient(Double_t x, Double_t y, Double_t z, HPointD& g)
{
  const Double_t xy = TMath::Sqrt(x*x + y*y);
  const Double_t r  = xy - mRM;
  g.x = 2.0 * mRminvsqr * (xy - mRM) / xy * x;
  g.y = 2.0 * mRminvsqr * (xy - mRM) / xy * y;
  g.z = 2.0 * mRminvsqr * z;
  return  mRminvsqr * (r*r + z*z);
}

void GTSTorus::GTSIsoEnd()
{}
