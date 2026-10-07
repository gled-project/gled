// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef RootGeo_ZGeoOvl_GL_RNR_H
#define RootGeo_ZGeoOvl_GL_RNR_H

#include <Glasses/ZGeoOvl.h>
#include <Rnr/GL/ZNode_GL_Rnr.h>
#include  "ZGeoNode_GL_Rnr.h"

namespace gled {

class ZGeoOvl_GL_Rnr : public ZGeoNode_GL_Rnr {
private:
  void _init();

protected:
  ZGeoOvl*	mZGeoOvl;

public:
  ZGeoOvl_GL_Rnr(ZGeoOvl* idol) : ZGeoNode_GL_Rnr(idol), mZGeoOvl(idol) { _init(); }

  virtual void Draw(RnrDriver* rd);
}; // endclass ZGeoOvl_GL_Rnr

} // endnamespace gled

#endif
