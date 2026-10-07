// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef Geom1_Sleeper_H
#define Geom1_Sleeper_H

#include <Glasses/Operator.h>

namespace gled {

class Sleeper : public Operator {
protected:
  UInt_t	mMSec;	// X{GS} 7 Value(-range=>[0,1e9,1,1])

public:
  Sleeper(const Text_t* n="Sleeper", const Text_t* t=0) : Operator(n,t) {}
  Sleeper(UInt_t ms, const Text_t* n="Sleeper", const Text_t* t=0) :
    Operator(n,t), mMSec(ms) {}

  // virtuals
  virtual void Operate(Operator::Arg* op_arg);

#include "Sleeper.h7"
  ClassDef(Sleeper, 1);
}; // endclass Sleeper


} // endnamespace gled

#endif
