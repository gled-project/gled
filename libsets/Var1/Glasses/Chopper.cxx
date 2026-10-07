// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "Chopper.h"

using namespace gled;

#include "Chopper.c7"

// Chopper

//______________________________________________________________________________
//
//

//==============================================================================

void Chopper::_init()
{}

Chopper::Chopper(const Text_t* n, const Text_t* t) :
  Flyer(n, t)
{
  _init();
}

Chopper::~Chopper()
{}

//==============================================================================
