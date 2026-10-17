// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef Geom1_WSWalker_H
#define Geom1_WSWalker_H

#include <Glasses/Operator.h>
#include <Stones/ZTrans.h>
#include <Stones/TimeMakerClient.h>

namespace gled {

class WSSeed;
class ZNode;

class WSWalker : public Operator, public TimeMakerClient
{
  MAC_RNR_FRIENDS(WSWalker);

private:
  void _init();

protected:
  ZLink<WSSeed>  mSeed; // X{GS} L{}
  ZLink<ZNode>   mNode; // X{GS} L{}

  Bool_t         bLoop; // X{GS} 7 Bool()

  // Flight: the node's up follows the felt force, acceleration minus gravity
  // (-z of the seed, in units per s^2), as in a coordinated turn, smoothed over
  // a time constant in s, while the force presses into the seat.
  Bool_t         bFlight;    // X{GS} 7 Bool(-join=>1)
  Double_t       mGravity;   // X{GS} 7 Value(-range=>[0,1e4,1,100], -join=>1)
  Double_t       mFlightTau; // X{GS} 7 Value(-range=>[0,10,1,1000])

  void set_trans(Double_t t);
  void fly(Double_t t);

  // Local cache
  Double_t m_last_time;  //!
  ZTrans   m_last_trans; //!
  ZPoint   m_up;         //!
  Bool_t   m_up_ok;      //!


public:
  WSWalker(const Text_t* n="WSWalker", const Text_t* t=0) :
    Operator(n,t) { _init(); }

  // Direct
  void SetTime(Double_t t);   // X{E} 7 MCWButt()

  // Operator
  virtual void Operate(Operator::Arg* op_arg);

  // TimeMakerClient
  virtual void TimeTick(Double_t t, Double_t dt);

#include "WSWalker.h7"
  ClassDef(WSWalker, 1); // Moves ZNode along a WeaverSymbol
}; // endclass WSWalker


} // endnamespace gled

#endif
