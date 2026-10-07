// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "Airplane.h"

using namespace gled;

#include "Airplane.c7"

// Airplane

//______________________________________________________________________________
//
//

//==============================================================================

void Airplane::_init()
{}

Airplane::Airplane(const Text_t* n, const Text_t* t) :
  Flyer(n, t)
{
  _init();
}

Airplane::~Airplane()
{}

//==============================================================================
