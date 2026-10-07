// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef GledCore_SSocket_H
#define GledCore_SSocket_H

#include "Gled/GMutex.h"
#include "TSocket.h"

namespace gled {

class SSocket : public TSocket
{
  friend class SServerSocket;

protected:
  GMutex   mMutex;
  Bool_t   mClosedDown;

public:
  SSocket() :
    TSocket(), mClosedDown(false) {}
  SSocket(TInetAddress address, const char *service, Int_t tcpwindowsize = -1) :
    TSocket(address, service, tcpwindowsize), mClosedDown(false) {}
  SSocket(TInetAddress address, Int_t port, Int_t tcpwindowsize = -1) :
    TSocket(address, port, tcpwindowsize), mClosedDown(false) {}
  SSocket(const char *host, const char *service, Int_t tcpwindowsize = -1) :
    TSocket(host, service, tcpwindowsize), mClosedDown(false) {}
  SSocket(const char *host, Int_t port, Int_t tcpwindowsize = -1) :
    TSocket(host, port, tcpwindowsize), mClosedDown(false) {}
  SSocket(const char *sockpath) :
    TSocket(sockpath), mClosedDown(false) {}
  SSocket(Int_t descriptor) :
    TSocket(descriptor), mClosedDown(false) {}
  SSocket(Int_t descriptor, const char *sockpath) :
    TSocket(descriptor, sockpath), mClosedDown(false) {}
  SSocket(const SSocket &s) :
    TSocket(s), mClosedDown(s.mClosedDown) {}
  virtual ~SSocket() {}

  virtual void Close(Option_t *opt="");

  ClassDef(SSocket, 0);
}; // endclass SSocket

} // endnamespace gled

#endif
