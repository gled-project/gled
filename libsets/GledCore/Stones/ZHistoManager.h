// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef GLED_ZHistoManager_H
#define GLED_ZHistoManager_H

#include <Gled/GledTypes.h>
#include <Stones/ZHistoDir.h>
#include <Stones/ZHisto.h>
#include <TFile.h>

namespace gled {

typedef std::map<ZHisto*, ZHistoDir*>		mHisto2ZHistoDir_t;
typedef std::map<ZHisto*, ZHistoDir*>::iterator	mHisto2ZHistoDir_i;

// ?? Should subclass SaturnService

class ZHistoManager : public TFile {
private:
  mHisto2ZHistoDir_t	mHisto2HistoDir;

public:
  ZHistoManager(const char* fname, Option_t* option="",
		const char* ftitle="", Int_t compress = 1);
  virtual ~ZHistoManager();

  void RegisterGroup(ZHisto *n, const Text_t* group);

  ZHistoDir*   GetDir(ZHisto* n);
  ZHistoGroup* GetGroup(ZHisto* n, const Text_t* group);

#include "ZHistoManager.h7"
  ClassDef(ZHistoManager, 0);
}; // endclass ZHistoManager

} // endnamespace gled

#endif
