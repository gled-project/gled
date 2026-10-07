// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef GledCore_SServerSocket_H
#define GledCore_SServerSocket_H

#include "SSocket.h"

namespace gled {

class SServerSocket : public SSocket
{
private:
  SServerSocket();
  SServerSocket(const SServerSocket &);
  void operator=(const SServerSocket &);

public:
  SServerSocket(Int_t port, Bool_t reuse=false, Int_t backlog=10, Int_t tcpwindowsize=-1);
  SServerSocket(const char *service, Bool_t reuse=false, Int_t backlog=10, Int_t tcpwindowsize= 1);
  virtual ~SServerSocket() {}

  virtual SSocket*      Accept();
  virtual TInetAddress  GetLocalInetAddress();
  virtual Int_t         GetLocalPort();

  ClassDef(SServerSocket, 0);
}; // endclass SServerSocket

} // endnamespace gled

#endif
