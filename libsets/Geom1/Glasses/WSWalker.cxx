// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

//__________________________________________________________________________
// WSWalker
//
// Moves mNode as instructed. mNode should be a child of mSeed (this
// is not checked anywhere). This restriction could be removed.
//
// 1) Direct SetTime
// 2) Operator loop   -> takes time from Eventor stack
// 3) TimeMakerClient -> as given by the argument

#include "WSWalker.h"
#include <Glasses/WSSeed.h>
#include <Glasses/ZNode.h>

#include <Glasses/Eventor.h>

using namespace gled;

#include "WSWalker.c7"

/**************************************************************************/

void WSWalker::_init()
{
  bLoop = false;
  bFlight = false; mGravity = 9.81; mFlightTau = 0.2;
  m_last_time = 1e30;
  m_up_ok = false;
}

/**************************************************************************/

void WSWalker::set_trans(Double_t t)
{
  if(mNode != 0 && mSeed != 0) {
    {
      GLensWriteHolder node_wrlck(*mSeed);
      mSeed->TransAtTime(m_last_trans, t, bLoop, (t < m_last_time));
      if(bFlight) fly(t);
    }
    {
      GLensWriteHolder node_wrlck(*mNode);
      mNode->SetTrans(m_last_trans);
    }
    m_last_time = t;
  }
}

void WSWalker::fly(Double_t t)
{
  // Under the seed's lock: replaces the up of m_last_trans.

  ZPoint pos, vel, acc;
  mSeed->MotionAtTime(t, pos, vel, acc, bLoop);

  const Double_t *ax = m_last_trans.ArrX(), *az = m_last_trans.ArrZ();
  const ZPoint x(ax[0], ax[1], ax[2]);
  ZPoint up = m_up_ok ? m_up : ZPoint(az[0], az[1], az[2]);

  // Where the felt force is weak or pulls out of the seat, as over a crest,
  // the up stays: airtime, not a roll.
  ZPoint f = acc;
  f.z += mGravity;
  f -= x * f.Dot(x);
  if(f.Dot(up) > 0.2*mGravity) {
    f.Normalize();
    const Double_t dt = t - m_last_time;
    const Double_t k  = (mFlightTau > 0 && dt >= 0 && dt < 1) ? 1 - TMath::Exp(-dt/mFlightTau) : 1;
    up += (f - up) * k;
  }
  up -= x * up.Dot(x);
  up.Normalize();

  m_last_trans.SetBaseVec(2, up.Cross(x));
  m_last_trans.SetBaseVec(3, up);
  m_up = up; m_up_ok = true;
}

/**************************************************************************/

void WSWalker::SetTime(Double_t t)
{
  set_trans(t);
}

/**************************************************************************/

void WSWalker::Operate(Operator::Arg* op_arg)
{
  Operator::PreOperate(op_arg);
  set_trans(op_arg->fEventor->GetEventTime());
  Operator::PostOperate(op_arg);
}

/**************************************************************************/

void WSWalker::TimeTick(Double_t t, Double_t dt)
{
  set_trans(t);
}
