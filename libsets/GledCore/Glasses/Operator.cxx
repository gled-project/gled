// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

//________________________________________________________________________
// Operator
//
// Base class for glasses that wish to participate in basic thread traversals.
// For now the implementation is simplified.
//
// In principle would have to be an `external' base (non-glass interface).
// Will be reimplemented in this spirit with the new reincarnation of p7.
//________________________________________________________________________

#include "Operator.h"

using namespace gled;

#include "Operator.c7"

void Operator::_init()
{
  bOpActive = bOpRecurse = true;
}

/**************************************************************************/

void Operator::ResetRecursively()
{
  if(bOpRecurse) {
    lpZGlass_t l; CopyList(l);
    for(lpZGlass_i i=l.begin(); i!=l.end(); ++i) {
      if(Operator* o = dynamic_cast<Operator*>(*i)) {
	if(o->bOpActive) o->ResetRecursively();
      }
    }
  }
}

/**************************************************************************/

void Operator::PreOperate(Operator::Arg* op_arg)
{
}

void Operator::Operate(Operator::Arg* op_arg)
{
  PreOperate(op_arg);
  // { ... do your stuff ... }
  PostOperate(op_arg);
}

void Operator::PostOperate(Operator::Arg* op_arg)
{
  if(bOpRecurse) {
    lpZGlass_t l; CopyList(l);
    if(op_arg->fUseDynCast) {
      for(lpZGlass_i i=l.begin(); i!=l.end(); ++i) {
	if(Operator* o = dynamic_cast<Operator*>(*i)) {
	  if(o->bOpActive) o->Operate(op_arg);
	}
      }
    } else {
      for(lpZGlass_i i=l.begin(); i!=l.end(); ++i) {
	Operator* o = (Operator*)(*i);
	if(o->bOpActive) o->Operate(op_arg);
      }
    }
  }
}
