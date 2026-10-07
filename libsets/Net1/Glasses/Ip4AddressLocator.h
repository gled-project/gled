// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef Net1_Ip4AddressLocator_H
#define Net1_Ip4AddressLocator_H

#include <Glasses/ZGlass.h>
#include <Stones/IpAddressLocation.h>

namespace gled {

class Ip4AddressLocator : public ZGlass
{
  MAC_RNR_FRIENDS(Ip4AddressLocator);

public:
  typedef std::vector<UInt_t> vUInt_t;
  typedef vUInt_t::iterator   vUInt_i;

  typedef std::vector<IpAddressLocation> vIpALoc_t;
  typedef vIpALoc_t::iterator            vIpALoc_i;

private:
  void _init();

protected:
  vUInt_t       mIpVec;
  vUInt_t       mLocIdxVec;

  vIpALoc_t     mLocInfoVec;

public:
  Ip4AddressLocator(const Text_t* n="Ip4AddressLocator", const Text_t* t=0);
  virtual ~Ip4AddressLocator();

  void LoadFromCsvFile(const TString& fname); // X{Ed} 7 MCWButt()

  void QueryHostIp(UInt_t ip4);

#include "Ip4AddressLocator.h7"
  ClassDef(Ip4AddressLocator, 1);
}; // endclass Ip4AddressLocator

} // endnamespace gled

#endif
