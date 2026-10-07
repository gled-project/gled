// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "SigTestOperator.h"

#include <Gled/GThread.h>

using namespace gled;

#include "SigTestOperator.c7"

// SigTestOperator

//______________________________________________________________________________
//
//

//==============================================================================

void SigTestOperator::_init()
{
  bRaiseILL = bRaiseBUS = bRaiseSEGV = bRaiseFPE = false;
  fResult = 0;
}

SigTestOperator::SigTestOperator(const Text_t* n, const Text_t* t) :
  Operator(n, t)
{
  _init();
}

SigTestOperator::~SigTestOperator()
{}

//==============================================================================

void SigTestOperator::Operate(Operator::Arg* op_arg)
{
  PreOperate(op_arg);

  if (bRaiseILL)
  {
    {
      GLensWriteHolder wrlck(this);
      SetRaiseILL(false);
    }
    // This just causes segv now :(
    // unsigned char insn[8] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
    // void (*function)() = (void (*)()) insn;
    // function();
    GThread::Self()->Kill(GThread::SigILL);
  }

  if (bRaiseBUS)
  {
    {
      GLensWriteHolder wrlck(this);
      SetRaiseBUS(false);
    }
    // This does not "work".
    // char     *cptr = (char*) malloc(2*sizeof(Long64_t));
    // Long64_t *iptr = (Long64_t*) (cptr + 3);
    // *iptr = 42;
    GThread::Self()->Kill(GThread::SigBUS);
  }

  if (bRaiseSEGV)
  {
    {
      GLensWriteHolder wrlck(this);
      SetRaiseSEGV(false);
    }
    // This causes ill on mac.
    // TObject* obj = 0;
    // obj->Print();
    GThread::Self()->Kill(GThread::SigSEGV);
  }

  if (bRaiseFPE)
  {
    {
      GLensWriteHolder wrlck(this);
      SetRaiseFPE(false);
    }
    Double_t zero = 0;
    fResult = 124343.5465 / zero;
  }

  PostOperate(op_arg);
}

