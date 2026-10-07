// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef GledCore_SigTestOperator_H
#define GledCore_SigTestOperator_H

#include <Glasses/Operator.h>

namespace gled {

class SigTestOperator : public Operator
{
  MAC_RNR_FRIENDS(SigTestOperator);

private:
  void _init();

protected:
  Bool_t     bRaiseILL;  //! X{GS} 7 Bool()
  Bool_t     bRaiseBUS;  //! X{GS} 7 Bool()
  Bool_t     bRaiseSEGV; //! X{GS} 7 Bool()
  Bool_t     bRaiseFPE;  //! X{GS} 7 Bool()
  Double_t   fResult;       //!

public:
  SigTestOperator(const Text_t* n="SigTestOperator", const Text_t* t=0);
  virtual ~SigTestOperator();

  virtual void Operate(Operator::Arg* op_arg);

#include "SigTestOperator.h7"
  ClassDef(SigTestOperator, 1);
}; // endclass SigTestOperator

} // endnamespace gled

#endif
