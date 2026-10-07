// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef GledCore_SRange_H
#define GledCore_SRange_H

#include <TString.h>

namespace gled {

class SRange
{
protected:
  Double_t	mMin;   // X{GS}
  Double_t	mMax;   // X{GS}
  Double_t      mSumX;  // X{GS}
  Double_t      mSumX2; // X{GS}
  ULong64_t     mN;     // X{GS}

public:
  SRange();

  void Reset();
  void Reset(Double_t min, Double_t max, Double_t sumx, Double_t sumx2, ULong64_t n);
  void SetSumX2FromSigma(Double_t sigma);

  void AddSample(Double_t x);

  Double_t GetAverage() const;
  Double_t GetSigma() const;

  void Dump(const TString& prefix="SRange: ", const TString& postfix="\n") const;

#include "SRange.h7"
  ClassDefNV(SRange, 1);
}; // endclass SRange

} // endnamespace gled

#endif
