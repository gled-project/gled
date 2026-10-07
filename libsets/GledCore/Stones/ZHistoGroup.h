// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef GLED_ZHistoGroup_H
#define GLED_ZHistoGroup_H

#include <Gled/GledTypes.h>
#include <TDirectory.h>
class TH1;
#include <vector>

namespace gled {

typedef std::vector<TH1**>			vppHisto_t;
typedef std::vector<TH1**>::iterator		vppHisto_i;

class ZHistoGroup {

protected:
  TDirectory*		mDir;		// X{g}

  vppHisto_t		mHistos;

public:
  ZHistoGroup(const Text_t* name, const Text_t* title);
  virtual ~ZHistoGroup() {}

  void AddHisto(TH1** p);
  void ResetHistos();

  void cd() { mDir->cd(); }

#include "ZHistoGroup.h7"
  ClassDef(ZHistoGroup, 0);
}; // endclass ZHistoGroup

// Here I declare callbacks for Draw, Reset ... should be somewhere else
// Perhaps namespace HistoFoo ?? ... better
void HistoDraw_cb(void* w, TObject* histo);
void HistoReset_cb(void* w, TObject* histo);

} // endnamespace gled

#endif
