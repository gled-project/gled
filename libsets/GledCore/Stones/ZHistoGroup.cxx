// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ZHistoGroup.h"
#include <TH1.h>

using namespace gled;


ZHistoGroup::ZHistoGroup(const Text_t* name, const Text_t* title)
{
  mDir = new TDirectory(name, title);
}

void ZHistoGroup::AddHisto(TH1** p) { mHistos.push_back(p); }

void ZHistoGroup::ResetHistos()
{
  for(vppHisto_i i=mHistos.begin(); i!=mHistos.end(); i++) {
    (*(*i))->Reset();
  }
}

/**************************************************************************/
// Histo callbacks
/**************************************************************************/

void HistoDraw_cb(void* w, TObject* histo) {
  if(TH1* h = dynamic_cast<TH1*>(histo)) {
    h->Draw();
  }
}

void HistoReset_cb(void* w, TObject* histo) {
  if(TH1* h = dynamic_cast<TH1*>(histo)) {
    h->Reset();
  }
}
