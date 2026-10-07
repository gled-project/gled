// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef Net1_UdpPacketTcpServer_H
#define Net1_UdpPacketTcpServer_H

#include "Glasses/ZGlass.h"
#include "Gled/GSelector.h"
#include "Gled/GQueue.h"

namespace gled {

class ZLog;
class UdpPacketSource;
class SUdpPacket;
class GThread;

class SServerSocket;
class SSocket;


class UdpPacketTcpServer : public ZGlass
{
  MAC_RNR_FRIENDS(UdpPacketTcpServer);

private:
  void _init();

protected:
  ZLink<ZLog>             mLog;         // X{GS} L{}
  ZLink<UdpPacketSource>  mSource;      // X{GS} L{}
  GQueue<SUdpPacket>      mUdpQueue;    //!

  //--------------------------------

  Int_t                   mServPort;    // X{GS} 7 Value()
  GSelector               mSelector;    //!
  GThread                *mServThread;  //!
  SServerSocket          *mServSocket;  //!
  std::list<SSocket*>     mClients;     //!

  static void* tl_Serve(UdpPacketTcpServer* s);
  void Serve();

  void AddClient(SSocket* cs);
  void RemoveClient(SSocket* cs);
  void RemoveClient(std::list<SSocket*>::iterator sli);

  //--------------------------------

  GThread          *mDeliThread;   //!

  static void* tl_Deliver(UdpPacketTcpServer* s);
  void Deliver();

public:
  UdpPacketTcpServer(const Text_t* n="UdpPacketTcpServer", const Text_t* t=0);
  virtual ~UdpPacketTcpServer();

  void StartAllServices(); // X{Ed} 7 MButt()
  void StopAllServices();  // X{Ed} 7 MButt()

#include "UdpPacketTcpServer.h7"
  ClassDef(UdpPacketTcpServer, 1);
}; // endclass UdpPacketTcpServer

} // endnamespace gled

#endif
