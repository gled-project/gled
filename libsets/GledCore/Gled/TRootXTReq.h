// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef GledCore_TRootXTReq_H
#define GledCore_TRootXTReq_H

#include <Gled/GledTypes.h>

#include <TString.h>

#include <functional>
#include <list>

class TTimer;
class TSignalHandler;

namespace gled {

class GMutex;
class GCondition;
class GThread;

class TRootXTReq
{
private:
  GCondition               *m_return_condition;

  static std::list<TRootXTReq*>  sQueue;
  static GThread           *sRootThread;
  static GMutex            *sQueueMutex;
  static bool               sSheduled;

  virtual void Act() = 0;

  static bool act_here();

protected:
  TString                   mName;

  void post_request();

public:
  TRootXTReq(const char* n="TRootXTReq");
  virtual ~TRootXTReq();

  void ShootRequest();
  void ShootRequestAndWait();

  static void Post(std::function<void()> foo);
  static void PostAndWait(std::function<void()> foo);

  // --- Static interface ---

  static void Bootstrap(GThread* root_thread);
  static void Shutdown();

  static void ProcessQueue();

#include "TRootXTReq.h7"
  ClassDef(TRootXTReq, 1);
}; // endclass TRootXTReq

} // endnamespace gled

#endif
