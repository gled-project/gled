// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef GledCore_ZNodeLink_GL_RNR_H
#define GledCore_ZNodeLink_GL_RNR_H

#include <Glasses/ZNodeLink.h>
#include <Rnr/GL/ZNode_GL_Rnr.h>

namespace gled {

class ZNodeLink_GL_Rnr : public ZNode_GL_Rnr {
private:
  void _init();
  void _setup_lens();
protected:
  ZNodeLink*	           mZNodeLink;
  OptoStructs::ZLinkDatum* mLensLD;

public:
  ZNodeLink_GL_Rnr(ZNodeLink* idol) :
    ZNode_GL_Rnr(idol), mZNodeLink(idol)
  { _init(); }

  virtual void SetImg(OptoStructs::ZGlassImg* newimg);

  virtual void CreateRnrScheme(RnrDriver* rd);

  virtual void PreDraw(RnrDriver* rd);
  virtual void PostDraw(RnrDriver* rd);

}; // endclass ZNodeLink_GL_Rnr

} // endnamespace gled

#endif
