// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef Geom1_ZGlShader_H
#define Geom1_ZGlShader_H

#include <Glasses/ZGlass.h>

#include <Eye/Ray.h>

namespace gled {

class ZGlShader : public ZGlass
{
  // 7777 AddViewInclude(GL/glew.h)
  // 7777 RnrCtrl(RnrBits(0,0,0,0, 0,0,0,0))
  MAC_RNR_FRIENDS(ZGlShader);
  MAC_RNR_FRIENDS(ZGlProgram);

public:
  enum PrivRayQN_e 
  {
    PRQN_offset = RayNS::RQN_user_0,
    PRQN_recompile
  };

private:
  void _init();

protected:
  Int_t         mType;      //  X{GS} 7 PhonyEnum(-vals=>[0,Undef, GL_VERTEX_SHADER,Vertex, GL_FRAGMENT_SHADER,Fragment])
  TString       mFile;      //  X{GS} 7 Filor()
  TString       mProgram;   //  X{GE}
  Bool_t        bAutoRecompile; // X{GS} 7 Bool()

  Bool_t        bCompiled;  //! X{GS} 7 BoolOut();
  TString       mLog;       //! X{S}

  GMutex        mProgMutex; //! Lock program text (used from GL).

  TString type_as_string();

public:
  ZGlShader(const Text_t* n="ZGlShader", const Text_t* t=0);
  virtual ~ZGlShader();

  void SetProgram(const Text_t* s);
  void EmitRecompileRay(); // X{E} 7 MButt()

  void Load(const Text_t* file=0); // X{E} 7 MButt(-join=>1)
  void Save(const Text_t* file=0); // X{E} 7 MCWButt()

  void PrintProgram(); //! X{E} 7 MButt()
  void PrintLog();     //! X{E} 7 MButt()

#include "ZGlShader.h7"
  ClassDef(ZGlShader, 1);
}; // endclass ZGlShader

} // endnamespace gled

#endif
