// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef GledCore_SaturnObserver_H
#define GledCore_SaturnObserver_H

#include <Glasses/Operator.h>

namespace gled {
class SaturnInfo;

class SaturnObserver : public Operator
{
  MAC_RNR_FRIENDS(SaturnObserver);

private:
  void _init();

protected:
  ZLink<SaturnInfo>	mTarget;	// X{gS} L{}

public:
  SaturnObserver(const Text_t* n="SaturnObserver", const Text_t* t=0) : Operator(n,t) { _init(); }

  virtual void Operate(Operator::Arg* op_arg);

#include "SaturnObserver.h7"
  ClassDef(SaturnObserver, 1);
}; // endclass SaturnObserver


} // endnamespace gled

#endif
