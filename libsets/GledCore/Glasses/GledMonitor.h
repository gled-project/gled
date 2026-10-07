// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef Gled_GledMonitor_H
#define Gled_GledMonitor_H

#include <Glasses/Operator.h>
#include <Stones/ZHisto.h>

#include <TH1F.h>

namespace gled {

class GledMonitor : public Operator, public ZHisto
{
  MAC_RNR_FRIENDS(GledMonitor);

private:
  void _init();

protected:
  Int_t         mCpuSampleTime; // X{GS} 7 Value(-range=>[0, 10000, 1])
  Bool_t	bFillHistos;	// X{GS} 7 Bool()

  // 7777 InstallHandler(GLED::Histo);

  TH1F*		h1LAvg1;  // X{gs} H7_LAvg("%m","%m for %c", 100, 0, 10)
  TH1F*		h1LAvg5;  // X{gs} H7_LAvg("%m","%m for %c", 100, 0, 10)
  TH1F*		h1LAvg15; // X{gs} H7_LAvg("%m","%m for %c", 100, 0, 10)

public:
  GledMonitor(const Text_t* n="GledMonitor", const Text_t* t=0) :
    Operator(n,t) { _init(); }

  virtual void AdEnlightenment();
  virtual void Operate(Operator::Arg* op_arg);

#include "GledMonitor.h7"
  ClassDef(GledMonitor, 1);
}; // endclass GledMonitor


} // endnamespace gled

#endif
