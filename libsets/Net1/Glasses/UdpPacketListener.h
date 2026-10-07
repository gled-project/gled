// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef Net1_UdpPacketListener_H
#define Net1_UdpPacketListener_H

#include <Glasses/UdpPacketSource.h>

namespace gled {

class UdpPacketListener : public UdpPacketSource
{
  MAC_RNR_FRIENDS(UdpPacketListener);

private:
  void _init();

protected:
  Int_t             mSuckPort;     // X{GS} 7 Value()
  Int_t             mSocket;       //!
  GThread          *mSuckerThread; //!

  static void* tl_Suck(UdpPacketListener* s);
  void Suck();


public:
  UdpPacketListener(const Text_t* n="UdpPacketListener", const Text_t* t=0);
  virtual ~UdpPacketListener();

  void StartAllServices(); // X{Ed} 7 MButt()
  void StopAllServices();  // X{Ed} 7 MButt()

#include "UdpPacketListener.h7"
  ClassDef(UdpPacketListener, 1);
}; // endclass UdpPacketListener

} // endnamespace gled

#endif
