// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef Net1_SUdpPacket_H
#define Net1_SUdpPacket_H

#include <Rtypes.h>

#include "Gled/GTime.h"
#include "Stones/SRefCounted.h"

class TBuffer;

namespace gled {

class SUdpPacket : public SRefCountedNV

{
  // Not implemented
public:
  GTime           mRecvTime;
  Int_t           mBuffLen;
  UShort_t        mAddrLen;
  UShort_t        mPort;
  UChar_t         mAddr[16];
  UChar_t        *mBuff;      //[mBuffLen]

  SUdpPacket();
  SUdpPacket(const GTime& t, UChar_t* addr, UShort_t addr_len, UShort_t port,
             UChar_t* buff, Int_t buff_len);
  ~SUdpPacket();

  SRefCountedNV_DecRefCount_macro

  void  NetStreamer(TBuffer& b);
  Int_t NetBufferSize() const;

  UInt_t Ip4AsUInt() const;

#include "SUdpPacket.h7"
  ClassDefNV(SUdpPacket, 1);
}; // endclass SUdpPacket

} // endnamespace gled

#endif
