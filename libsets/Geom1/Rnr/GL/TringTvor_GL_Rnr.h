// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef Geom1_TringTvor_GL_Rnr_H
#define Geom1_TringTvor_GL_Rnr_H

#include <Stones/TringTvor.h>

namespace gled {

class TringTvor_GL_Rnr
{
protected:
  TringTvor            &mTT;

  Float_t              *mNArr;
  UChar_t              *mCArr;
  Float_t              *mTArr;

  TringTvor::Mode_e     mNMode;
  TringTvor::Mode_e     mCMode;
  TringTvor::Mode_e     mTMode;

  enum RenderMode_e { RM_Unknown, RM_Smooth, RM_Flat, RM_Mixed };

  RenderMode_e mRenderMode;

  Bool_t       bTriangleStripsRequested;
  Bool_t       bTriangleStrips;

  UChar_t* TriangleColor(Int_t i) const { return &(mCArr[4*i]); }

public:
  TringTvor_GL_Rnr(TringTvor* tvor);

  // Setup functions

  void BeginSetup();

  void SetupStandard      (Bool_t color_p=true, Bool_t texture_p=true);
  void SetupStandardSmooth(Bool_t color_p=true, Bool_t texture_p=true);
  void SetupStandardFlat  (Bool_t color_p=true, Bool_t texture_p=true);

  void SetNormalArray (TringTvor::Mode_e mode, Float_t* narr=0);
  void SetColorArray  (TringTvor::Mode_e mode, UChar_t* carr=0);
  void SetTextureArray(TringTvor::Mode_e mode, Float_t* tarr=0);

  void RequestTriangleStrips();

  void EndSetup();

  // Render functions

  void BeginRender();
  void Render();
  void RenderSmooth();
  void RenderFlat();
  void RenderMixed();
  void EndRender();

  // Static, all inclusive methods

  static void Render(TringTvor* ttvor);
  static void Render(TringTvor* ttvor, Bool_t smoothp, Bool_t colp=true, Bool_t texp=true);

  static void RenderCEBBox(const Float_t* x, Float_t f=1.0f, Bool_t pushpop=true);

}; // endclass TringTvor_GL_Rnr

} // endnamespace gled

#endif
