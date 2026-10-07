// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef RootGeo_GeoUserData_H
#define RootGeo_GeoUserData_H

#include <TObject.h>
class TGLFaceSet;

namespace gled {

class GeoUserData : public TObject {

private:
  void _init();

protected:

public:
  Bool_t	bIsImported;
  TGLFaceSet*	fFaceSet;

  GeoUserData(Bool_t impp=false, TGLFaceSet *fs=0) :
    bIsImported(impp),fFaceSet(fs)
  { _init(); }

#include "GeoUserData.h7"
  ClassDef(GeoUserData, 1);
}; // endclass GeoUserData

} // endnamespace gled

#endif
