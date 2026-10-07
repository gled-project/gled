// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef Net1_IpAddressLocation_H
#define Net1_IpAddressLocation_H

#include <Rtypes.h>
#include <TString.h>

namespace gled {

class IpAddressLocation
{
protected:
  TString  mCity;       // X{GR}
  TString  mState;      // X{GR}
  // TString  mCountry;    // X{GR}
  Double_t mLatitude;   // X{GR}
  Double_t mLongitude;  // X{GR}

public:
  IpAddressLocation() : mLatitude(0), mLongitude(0) {}
  IpAddressLocation(const TString& loc);
  virtual ~IpAddressLocation();

#include "IpAddressLocation.h7"
  ClassDefNV(IpAddressLocation, 1);
}; // endclass IpAddressLocation

} // endnamespace gled

#endif
