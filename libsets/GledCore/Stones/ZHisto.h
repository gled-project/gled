// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef GLED_ZHisto_H
#define GLED_ZHisto_H

#include <Gled/GledTypes.h>
#include <TString.h>

namespace gled {
class ZGlass;
class ZHistoManager;
class ZHistoDir;

class ZHisto {

protected:
  ZHistoManager*	mManager;	//! X{gs}

public:
  ZHisto(ZHistoManager* m=0) : mManager(m) {}
  virtual ~ZHisto() {}

  virtual ZGlass* GetGlass();
  virtual ZHistoManager* GetZHistoManager();
  ZHistoDir* GetHistoDir();

  virtual void InitHistoGroups() = 0;
  virtual void ResetHistos() = 0;

#include "ZHisto.h7"
  ClassDef(ZHisto, 1);
}; // endclass ZHisto

} // endnamespace gled

#endif
