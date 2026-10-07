// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef RootGeo_ZGeoOvl_H
#define RootGeo_ZGeoOvl_H

#include <Glasses/ZGeoNode.h>

namespace gled {

class ZGeoOvl : public ZGeoNode {

  // 7777 RnrCtrl(RnrBits(2,4,6,0, 0,0,0,3))
  MAC_RNR_FRIENDS(ZGeoOvl);
  friend class ZGeoOvlMgr;

private:
  void _init();

protected:
  Bool_t    mIsExtr;    // X{GS}
  Double_t  mOverlap;   // X{GS}  7 ValOut()
  Int_t     mPM_N;      // X{GS}
  Float_t*  mPM_p;      //[mPM_N*3] X{gS}
  ZColor    mPM_Col;    // X{GSP}
  Bool_t    mRnrMark;   // X{GS}  7 Bool(-join=>1)
  Bool_t    mRnrNode;   // X{GS}  7 Bool()

public:
  ZGeoOvl(const Text_t* n="ZGeoOvl", const Text_t* t=0) : ZGeoNode(n,t)
  { _init(); }

  using ZGeoNode::Restore;
  virtual void Restore(TGeoVolume* vol);

  void DumpOvl(); //! X{E} 7 MButt()

#include "ZGeoOvl.h7"
  ClassDef(ZGeoOvl, 1);
}; // endclass ZGeoOvl

} // endnamespace gled

#endif
