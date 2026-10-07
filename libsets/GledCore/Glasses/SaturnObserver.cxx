// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

//__________________________________________________________________________
// SaturnObserver
//
//

#include "SaturnObserver.h"
#include "Glasses/SaturnInfo.h"

using namespace gled;

#include "SaturnObserver.c7"

/**************************************************************************/

void SaturnObserver::_init()
{}

/**************************************************************************/

void SaturnObserver::Operate(Operator::Arg* op_arg)
{
  Operator::PreOperate(op_arg);

  if(mTarget != 0) {
    std::unique_ptr<ZMIR> mir( mTarget->S_TellAverages() );
    mir->SetRecipient(mTarget.get());
    std::unique_ptr<ZMIR_RR> ret ( mSaturn->ShootMIRWaitResult(mir) );
    if(ret->BeamResult_OK()) {
      Float_t lavg[3];
      *ret >> lavg[0] >> lavg[1] >> lavg[2];
      printf("Got averages: %6.2f %6.2f %6.2f\n", lavg[0], lavg[1], lavg[2]);
    }
  }

  Operator::PostOperate(op_arg);
}
