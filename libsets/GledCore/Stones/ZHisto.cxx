// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ZHisto.h"
#include <Stones/ZHistoManager.h>
#include <Glasses/ZGlass.h>
#include <Ephra/Saturn.h>

using namespace gled;


ZGlass* ZHisto::GetGlass()
{
  return dynamic_cast<ZGlass*>(this);
}

ZHistoManager* ZHisto::GetZHistoManager()
{
  if(mManager) return mManager;
  return GetGlass()->GetSaturn()->GetZHistoManager();
}

ZHistoDir* ZHisto::GetHistoDir()
{
  return GetZHistoManager()->GetDir(this);
}
