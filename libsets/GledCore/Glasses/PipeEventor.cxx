// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

//__________________________________________________________________________
// PipeEventor
//
// Spawns mCommand via gSystem->PipeOpen() in write mode.
// You can send data to be written to the pipe with SendCommand().
//

#include "PipeEventor.h"
#include <Gled/GThread.h>

#include <TSystem.h>

#include <signal.h>

using namespace gled;

#include "PipeEventor.c7"

/**************************************************************************/

void PipeEventor::_init()
{
  mPipe = 0;
  mWaitTimeMS = 1000;

  bSignalSafe = true;
}

/**************************************************************************/

void PipeEventor::OnStart(Operator::Arg* op_arg)
{
  Eventor::OnStart(op_arg);

  sigset_t set;

  sigemptyset(&set);
  sigaddset(&set, GThread::SigPIPE);
  pthread_sigmask(SIG_BLOCK, &set, 0);

  mPipe = gSystem->OpenPipe(mCommand.Data(), "w");
  if(!mPipe) {
    ISerr(GForm("PipeEventor::OnStart [%s] OpenPipe(%s) failed",
		GetName(), mCommand.Data()));
    return;
  }
  setlinebuf(mPipe);
}

void PipeEventor::OnExit(Operator::Arg* op_arg)
{
  if (mPipe)
  {
    gSystem->ClosePipe(mPipe);
    mPipe = 0;
  }
  ClearPendingCommands();

  Eventor::OnExit(op_arg);
}

/**************************************************************************/

void PipeEventor::Operate(Operator::Arg* op_arg)
{
  if (!mPipe)
  {
    throw Operator::Exception(this, Operator::OE_Break, "pipe closed");
  }

  mSendCond.Lock();
  try {
    feed_commands();
    mSendCond.TimedWait(GTime::MiliSec(mWaitTimeMS));
    feed_commands();
  }
  catch(Operator::Exception &exc) {
    mSendCond.Unlock();
    throw;
  }
  mSendCond.Unlock();
}

/**************************************************************************/

void PipeEventor::PostCommand(TString& command)
{
  mSendCond.Lock();
  mPending.push_back(command);
  mSendCond.Signal();
  mSendCond.Unlock();
}

void PipeEventor::ClearPendingCommands()
{
  mSendCond.Lock();
  mPending.clear();
  mSendCond.Unlock();
}
/**************************************************************************/
// Protected methods
/**************************************************************************/

void PipeEventor::feed_commands()
{
  // Feeds commands (if any) from mPending to mPipe.
  // Caller must lock the mSendCond.

  if(ferror(mPipe)) {
    throw( Operator::Exception(this, Operator::OE_Break, "pipe closed") );
  }

  while(!mPending.empty()) {
    int ret = fprintf(mPipe, "%s\n", mPending.front().Data());
    if(ret < 0 || ferror(mPipe)) {
      throw( Operator::Exception(this, Operator::OE_Break, "write error") );
    }
    mPending.pop_front();
  }
}


/**************************************************************************/
