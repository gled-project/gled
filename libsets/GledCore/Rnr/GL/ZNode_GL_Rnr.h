// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef Gled_ZNode_GL_Rnr
#define Gled_ZNode_GL_Rnr

#include <Glasses/ZNode.h>
#include <Rnr/GL/ZGlass_GL_Rnr.h>
#include <RnrBase/PMSEntry.h>

namespace gled {
class RnrDriver;

class ZNode_GL_Rnr : public ZGlass_GL_Rnr
{
  void _init();
  void _setup_rnrmod();
protected:
  ZNode*        mNode;
  TimeStamp_t   mStampTrans;

  PMSEntry      mPMSE;

  bool          bNormP;
  bool          bNormWasOffP;
  Float_t       mExDOM;

  OptoStructs::ZLinkDatum* mRnrModLD;

public:
  ZNode_GL_Rnr(ZNode* n) : ZGlass_GL_Rnr(n), mNode(n), mStampTrans(0)
  { _init(); }

  virtual void SetImg(OptoStructs::ZGlassImg* newimg);

  virtual void CreateRnrScheme(RnrDriver* rd);

  virtual void PreDraw(RnrDriver* rd);
  virtual void PostDraw(RnrDriver* rd);

}; // endclass ZNode_GL_Rnr

} // endnamespace gled

#endif
