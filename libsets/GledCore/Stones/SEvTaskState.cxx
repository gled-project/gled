// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

//__________________________________________________________________________
// SEvTaskState
//
//

#include "SEvTaskState.h"

using namespace gled;


/**************************************************************************/

void SEvTaskState::_init()
{
  mState = 'W';
  mNAll = mNOK = mNFail = mNProc = 0;
}

void SEvTaskState::Reinit()
{
  Reinit(mNAll);
}

void SEvTaskState::Reinit(Int_t n)
{
  _init();
  mNAll = n;
}

void SEvTaskState::Finalize()
{
  mNProc = 0;
  mNFail = mNAll - mNOK;
  mState = 'F';
}

/**************************************************************************/

SEvTaskState& SEvTaskState::operator+=(const SEvTaskState& s)
{
  mNAll  += s.mNAll;  mNOK   += s.mNOK;
  mNFail += s.mNFail; mNProc += s.mNProc;
  return *this;
}


/**************************************************************************/
