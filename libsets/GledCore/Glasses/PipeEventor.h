// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef GledCore_PipeEventor_H
#define GledCore_PipeEventor_H

#include <Glasses/Eventor.h>

namespace gled {

class PipeEventor : public Eventor
{
  MAC_RNR_FRIENDS(PipeEventor);

private:
  void _init();

protected:
  TString	mCommand;	// X{GS} 7 Textor();
  FILE*		mPipe;		//!

  UInt_t	mWaitTimeMS;	// X{GS} 7 Value(-range=>[1,1000000,1,1]);

  GCondition	mSendCond;	//!

  lStr_t	mPending;	//!

  void feed_commands();

public:
  PipeEventor(const Text_t* n="PipeEventor", const Text_t* t=0)
    : Eventor(n,t), mSendCond(GMutex::recursive)
  { _init(); }

  virtual void OnStart(Operator::Arg* op_arg);
  virtual void OnExit(Operator::Arg* op_arg);

  virtual void Operate(Operator::Arg* op_arg);

  void PostCommand(TString& command);	// X{E}
  void ClearPendingCommands();		// X{E}

#include "PipeEventor.h7"
  ClassDef(PipeEventor, 1);
}; // endclass PipeEventor


} // endnamespace gled

#endif
