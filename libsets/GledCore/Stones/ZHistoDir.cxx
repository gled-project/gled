// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ZHistoDir.h"
#include <Glasses/ZGlass.h>
#include <Gled/GledNS.h>

using namespace gled;


ZHistoDir::ZHistoDir(ZHisto* n, const Text_t* name, const Text_t* title) :
  mHisto(n)
{
  mDir = new TDirectory(name, title);
}

ZHistoDir::~ZHistoDir() {}
/* this crashed root ...
  for(mName2HistoGroup_i i=mName2Group.begin(); i!=mName2Group.end(); i++)
    delete i->second;
*/

void ZHistoDir::AddGroup(const Text_t* name, const Text_t* title)
{
  if(mName2Group[name]) {
    ISerr(GForm("ZHistoDir::AddGroup Registering ZHistoGroup %s for %s *again* ... skip",
		name, mHisto->GetGlass()->GetName()));
    return;
  }
  GledNS::PushFD(); mDir->cd();
  mName2Group[name] = new ZHistoGroup(name, title);
  GledNS::PopFD();
}
