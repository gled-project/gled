// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

//__________________________________________________________________________
// PerfMeterOperator
//
// If bUseBeams is true, make sure mBeamHost IS set.
// In fact should do this somwhere in routing code ... but anyway ...

#include "PerfMeterOperator.h"
#include <Glasses/PerfMeterTarget.h>
#include <Glasses/SaturnInfo.h>

using namespace gled;

#include "PerfMeterOperator.c7"

/**************************************************************************/

void PerfMeterOperator::_init()
{
  mTest = TT_Void; bUseBeams = false;
  mVecSize  = 1;
  mTarget   = 0;
  mBeamHost = 0;
}

/**************************************************************************/

void PerfMeterOperator::Operate(Operator::Arg* op_arg)
{
  Operator::PreOperate(op_arg);

  if (mTarget == 0) goto end_operate;

  switch (mTest)
  {
    case TT_Void:
    {
      break;
    }
    case TT_Null:
    {
      if (op_arg->fMultix)
      {
	mTarget->NullMethod();
      }
      else
      {
 std::unique_ptr<ZMIR> mir(mTarget->S_NullMethod());
	send_beam_or_flare(mir);
      }
      break;
    }
    case TT_IncCount:
    {
      if (op_arg->fMultix)
      {
	mTarget->IncCount();
      }
      else
      {
 std::unique_ptr<ZMIR> mir(mTarget->S_IncCount());
	send_beam_or_flare(mir);
      }
      break;
    }
    case TT_SetVector:
    {
      TVector vec(mVecSize);
      if (op_arg->fMultix)
      {
	mTarget->AssignVector(vec);
      }
      else
      {
 std::unique_ptr<ZMIR> mir(mTarget->S_AssignVector(vec));
	send_beam_or_flare(mir);
      }
      break;
    }
  }

end_operate:
  Operator::PostOperate(op_arg);
}

/**************************************************************************/

void PerfMeterOperator::send_beam_or_flare(std::unique_ptr<ZMIR>& m)
{
  if(bUseBeams) {
    SaturnInfo *rec = mBeamHost.is_set() ? mBeamHost.get() : mSaturn->GetSaturnInfo();
    m->SetRecipient(rec);
  }
  mSaturn->PostMIR(m);
}
