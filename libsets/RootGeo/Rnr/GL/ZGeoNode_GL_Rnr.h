// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef RootGeo_ZGeoNode_GL_RNR_H
#define RootGeo_ZGeoNode_GL_RNR_H

#include <Glasses/ZGeoNode.h>
#include <Rnr/GL/ZNode_GL_Rnr.h>
#include <Stones/GeoUserData.h>

namespace gled {

class ZGeoNode_GL_Rnr : public ZNode_GL_Rnr {
private:
  void _init();

protected:
  ZGeoNode*	mZGeoNode;

public:
  ZGeoNode_GL_Rnr(ZGeoNode* idol) : ZNode_GL_Rnr(idol), mZGeoNode(idol) { _init(); }

  virtual void Draw(RnrDriver* rd);

}; // endclass ZGeoNode_GL_Rnr

} // endnamespace gled

#endif
