// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef RootGeo_GeoUserData_H
#define RootGeo_GeoUserData_H

#include <TObject.h>

namespace gled {

class GeoMesh;

class GeoUserData : public TObject {

private:
  void _init();

protected:

public:
  Bool_t	bIsImported;
  GeoMesh*	fMesh;       //!

  GeoUserData(Bool_t impp=false) :
    bIsImported(impp), fMesh(0)
  { _init(); }
  virtual ~GeoUserData();

#include "GeoUserData.h7"
  ClassDef(GeoUserData, 1);
}; // endclass GeoUserData

} // endnamespace gled

#endif
