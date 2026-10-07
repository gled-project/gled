// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ExtendioSpiritio.h"
#include <Glasses/Extendio.h>

using namespace gled;

#include "ExtendioSpiritio.c7"

// ExtendioSpiritio

//______________________________________________________________________________
//
//

//==============================================================================

void ExtendioSpiritio::_init()
{}

ExtendioSpiritio::ExtendioSpiritio(const Text_t* n, const Text_t* t) :
  Spiritio(n, t)
{
  _init();
}

ExtendioSpiritio::~ExtendioSpiritio()
{}

//==============================================================================
