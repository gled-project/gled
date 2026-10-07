// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef GledCore_Mountain_H
#define GledCore_Mountain_H

#include <Glasses/Eventor.h>
#include <Gled/GThread.h>
#include <Gled/GMutex.h>
#include <Gled/GCondition.h>
#include <Gled/GTime.h>

namespace gled {

class Mountain;

struct DancerInfo
{
  GThread*		fThread;
  Eventor*		fEventor;
  Mountain*		fMountain;
  Operator::Arg* 	fOpArg;

  bool			fSuspended;
  bool			fSleeping;
  bool			fShouldSuspend;
  bool			fShouldExit;

  DancerInfo(GThread* t, Eventor* e, Mountain* m) :
    fThread(t), fEventor(e), fMountain(m), fOpArg(0),
    fSuspended(false), fSleeping(false),
    fShouldSuspend(false), fShouldExit(false)
  {}

  ~DancerInfo() {}
};

typedef std::unordered_map<Eventor*, DancerInfo*>			hEv2DI_t;
typedef std::unordered_map<Eventor*, DancerInfo*>::iterator	hEv2DI_i;


class Mountain
{
private:
  GMutex		hStageLock;
  GCondition		hSuspendCond;
  bool			bInSuspend;
  UInt_t		hSuspendCount;

protected:
  hEv2DI_t		hOnStage;
  Saturn*		mSaturn;	// X{g}

  void stop_thread(DancerInfo* di);

public:
  Mountain(Saturn* s) : hStageLock(GMutex::recursive), mSaturn(s)
  { bInSuspend = false; }
  virtual ~Mountain() {}

  // Eventor request handling
  void Start(Eventor* e, bool suspend_immediately=false);
  void Stop(Eventor* e);
  void Suspend(Eventor* e);
  void Resume(Eventor* e);
  void Cancel(Eventor* e);

  DancerInfo* UnregisterThread(Eventor* e);
  void WipeThread(Eventor* e);

  // Suspension mechanism
  Int_t SuspendAll();
  void 	ResumeAll();
  void 	ConsiderSuspend(DancerInfo* di);

  // Shutdown ... kill all threads
  void Shutdown();

  static void  DancerCooler(DancerInfo* di);
  static void* DancerBeat(DancerInfo* di);

#include "Mountain.h7"
  ClassDef(Mountain, 0);
}; // endclass Mountain

} // endnamespace gled

#endif
