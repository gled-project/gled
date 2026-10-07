// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef Geom1_ZGlProgram_H
#define Geom1_ZGlProgram_H

#include <Glasses/ZList.h>

#include <Eye/Ray.h>

namespace gled {

class SGlUniform;

class ZGlProgram : public ZList
{
  // 7777 RnrCtrl(RnrBits(0,4,0,0, 0,0,0,0))
  MAC_RNR_FRIENDS(ZGlProgram);

public:
  enum PrivRayQN_e 
  {
    PRQN_offset = RayNS::RQN_user_0,
    PRQN_relink,
    PRQN_rebuild
  };

  typedef std::map<TString, SGlUniform*>        mName2pUniform_t;
  typedef mName2pUniform_t::iterator       mName2pUniform_i;
  typedef mName2pUniform_t::const_iterator mName2pUniform_ci;

private:
  void _init();

protected:
  Bool_t            bSetUniDefaults;  //  X{GS} 7 Bool()

  Bool_t            bLinked;          //! X{GS} 7 BoolOut();
  TString           mLog;             //! X{S}

  GMutex            mUniMutex;        //! X{R}
  mName2pUniform_t  mUniMap;          //! X{R}

  void swap_unimap(mName2pUniform_t& umap);

public:
  ZGlProgram(const Text_t* n="ZGlProgram", const Text_t* t=0);
  virtual ~ZGlProgram();

  void EmitRelinkRay();    // X{E}  7 MButt()
  void EmitRebuildRay();   // X{E}  7 MButt()
  void ReloadAndRebuild(); // X{ED} 7 MButt()

  void PrintLog(); //! X{E} 7 MButt()

  void PrintUniforms(); //! X{E} 7 MButt()

#include "ZGlProgram.h7"
  ClassDef(ZGlProgram, 1);
}; // endclass ZGlProgram

} // endnamespace gled

#endif
