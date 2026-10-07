// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "UdpPacketSource.h"
#include "Glasses/ZLog.h"

// UdpPacketSource

//______________________________________________________________________________
//
//

using namespace gled;
#include "UdpPacketSource.c7"

//==============================================================================

void UdpPacketSource::_init()
{}

UdpPacketSource::UdpPacketSource(const Text_t* n, const Text_t* t) :
  ZGlass(n, t)
{
  _init();
}

UdpPacketSource::~UdpPacketSource()
{}

//==============================================================================

void UdpPacketSource::RegisterConsumer(Queue_t* q)
{
  mConsumerSet.RegisterQueue(q);
}

void UdpPacketSource::UnregisterConsumer(Queue_t* q)
{
  mConsumerSet.UnregisterQueue(q);
}
