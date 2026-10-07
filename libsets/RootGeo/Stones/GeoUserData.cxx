// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

//__________________________________________________________________________
// GeoUserData
//
//

#include "GeoUserData.h"
#include "GeoMesh.h"

using namespace gled;

/**************************************************************************/

void GeoUserData::_init()
{}

GeoUserData::~GeoUserData()
{
  delete fMesh;
}

/**************************************************************************/
