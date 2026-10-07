// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef GledCore_ZGlMaterial_H
#define GledCore_ZGlMaterial_H

#include <Glasses/ZRnrModBase.h>
#include <Stones/ZColor.h>

namespace gled {

class ZGlMaterial : public ZRnrModBase
{
  // 7777 AddViewInclude(GL/glew.h)
  // 7777 RnrCtrl(RnrBits(0,4,0,0))
  MAC_RNR_FRIENDS(ZGlMaterial);

private:
  void _init();

protected:
  // Material properties
  ZRnrModBase::Operation_e
                mMatOp;     // X{GS}  7 PhonyEnum()
  Int_t		mFace;	    // X{GS}  7 PhonyEnum(-vals=>[GL_FRONT,Front, GL_BACK,Back, GL_FRONT_AND_BACK,FrontAndBack], -width=>10, -join=>1)
  Float_t	mShininess; // X{GS}  7 Value(-range=>[0,1024,1,100], -width=>4)
  ZColor	mAmbient;   // X{PGS} 7 ColorButt(-join=>1)
  ZColor	mDiffuse;   // X{PGS} 7 ColorButt()
  ZColor	mSpecular;  // X{PGS} 7 ColorButt(-join=>1)
  ZColor	mEmission;  // X{PGS} 7 ColorButt()

  // Material mode
  ZRnrModBase::Operation_e
                mModeOp;    // X{GS}  7 PhonyEnum()
  Int_t         mModeFace;  // X{GS}  7 PhonyEnum(-vals=>[GL_FRONT,Front, GL_BACK,Back, GL_FRONT_AND_BACK,FrontAndBack], -width=>10, -join=>1)
  Int_t		mModeColor; // X{GS}  7 PhonyEnum(-vals=>[GL_AMBIENT,Ambient, GL_DIFFUSE,Diffuse, GL_SPECULAR,Specular, GL_AMBIENT_AND_DIFFUSE,AmbAndDiff, GL_EMISSION,Emission], -width=>10)
public:
  ZGlMaterial(const Text_t* n="ZGlMaterial", const Text_t* t=0) : ZRnrModBase(n,t) { _init(); }


#include "ZGlMaterial.h7"
  ClassDef(ZGlMaterial, 1); // Control of GL material colors and mode of application of current color
}; // endclass ZGlMaterial


} // endnamespace gled

#endif
