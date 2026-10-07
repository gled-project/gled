// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ZHistoManager.h"
#include <Gled/GledNS.h>
#include <Glasses/ZGlass.h>

using namespace gled;


ZHistoManager::ZHistoManager(const char* fname, Option_t* option,
			     const char* ftitle, Int_t compress) :
  TFile(fname, option, ftitle, compress)
{}

ZHistoManager::~ZHistoManager() {}

void ZHistoManager::RegisterGroup(ZHisto* n, const Text_t* group)
{
  mHisto2ZHistoDir_i i = mHisto2HistoDir.find(n);
  ZHistoDir* d = (i != mHisto2HistoDir.end()) ? i->second : 0;
  const Text_t* name = n->GetGlass()->GetName();
  GledNS::PushFD(); cd();
  if(!d) {
    ISmess(GForm("ZHistoManager::Register creating ZHistoDir for %s", name));
    d = mHisto2HistoDir[n] = new ZHistoDir(n, name, GForm("ZHistoDir for %s", name));
  }
  d->AddGroup(group, GForm("ZHistoGroup for %s", name));
  GledNS::PopFD();
}

/**************************************************************************/

ZHistoDir* ZHistoManager::GetDir(ZHisto* n)
{
  return mHisto2HistoDir[n];
}

ZHistoGroup* ZHistoManager::GetGroup(ZHisto* n, const Text_t* group)
{
  ZHistoDir *d = mHisto2HistoDir[n];
  return (d ? d->GetGroup(group) : 0);
}
