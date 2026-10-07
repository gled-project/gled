// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "TringuObserver.h"

using namespace gled;

#include "TringuObserver.c7"

// TringuObserver

//______________________________________________________________________________
//
//

//==============================================================================

void TringuObserver::_init()
{}

TringuObserver::TringuObserver(const Text_t* n, const Text_t* t) :
  ZNode(n, t)
{
  _init();
}

TringuObserver::~TringuObserver()
{}

//==============================================================================
