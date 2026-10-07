// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "TubeTvor.h"

using namespace gled;


TubeTvor::TubeTvor() : mV(0), mN(0), mC(0), mT(0),
		       bColP(false), bTexP(false)
{}

TubeTvor::~TubeTvor()
{
  delete [] mV; delete [] mN; delete [] mC; delete [] mT;
}

void TubeTvor::Init(Int_t npoles, Int_t nrings, Int_t nphi,
		    Bool_t colp, Bool_t texp)
{
  delete [] mV; delete [] mN; delete [] mC; delete [] mT;
  bColP = colp;
  bTexP = texp;

  mNP = npoles + (nphi+1)*nrings;
  mI = 0;
  mRings.clear();
  mV = new Float_t[mNP*3];
  mN = new Float_t[mNP*3];
  if(bColP) mC = new UChar_t[mNP*4]; else mC = 0;
  if(bTexP) mT = new Float_t[mNP*2]; else mT = 0;
}

void TubeTvor::NewRing(Int_t n, Bool_t dp)
{
  mRings.push_back( RingInfo(mI, n, dp) );
}
