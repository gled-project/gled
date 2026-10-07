// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

//__________________________________________________________________________
// ZFireQueen
//
//

#include "ZFireQueen.h"
#include "ZHashList.h"
#include "ZEunuch.h"

using namespace gled;

#include "ZFireQueen.c7"

/**************************************************************************/

void ZFireQueen::_init()
{
  mEunuchs = 0;
}

/**************************************************************************/

void ZFireQueen::bootstrap()
{
  PARENT_GLASS::bootstrap();

  ZHashList* l = new ZHashList("Eunuchs");
  l->SetElementFID(ZEunuch::FID());
  CheckIn(l); SetEunuchs(l); l->SetMIRActive(false);

  ZNameMap* nm = new ZNameMap("Etc");
  CheckIn(nm); Add(nm);
}

/**************************************************************************/

ZGlass* ZFireQueen::DemangleID(ID_t id){
  // This should serve to properly demangle external references for comets.
  // FireQueens ignore dependencies.

  return mSaturn->DemangleID(id);
}
