// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef GLED_ZHistoDir_H
#define GLED_ZHistoDir_H

#include <Gled/GledTypes.h>
#include <Stones/ZHisto.h>
#include <Stones/ZHistoGroup.h>
#include <TDirectory.h>

namespace gled {

typedef std::map<const Text_t*, ZHistoGroup*>		mName2HistoGroup_t;
typedef std::map<const Text_t*, ZHistoGroup*>::iterator	mName2HistoGroup_i;

class ZHistoDir {

protected:
  TDirectory*		mDir;		// X{g}

  ZHisto*		mHisto;		// X{g}
  mName2HistoGroup_t	mName2Group;

public:
  ZHistoDir(ZHisto* n, const Text_t* name, const Text_t* title);
  virtual ~ZHistoDir();

  void AddGroup(const Text_t* name, const Text_t* title);
  ZHistoGroup* GetGroup(const Text_t* name) { return mName2Group[name]; }

  void cd() { mDir->cd(); }

#include "ZHistoDir.h7"
  ClassDef(ZHistoDir, 0);
}; // endclass ZHistoDir

} // endnamespace gled

#endif
