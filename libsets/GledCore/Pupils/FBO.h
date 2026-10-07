// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef GledCore_FBO_H
#define GledCore_FBO_H

#include <Gled/GledTypes.h>

namespace gled {

class FBO {
public:
  UInt_t  fFrameBuffer;
  UInt_t  fColorTexture;
  UInt_t  fDepthBuffer;
  // UInt_t  fStencilBuffer;
  Int_t   fW, fH;

  Bool_t  fIsRescaled;
  Float_t fWScale, fHScale;

  static Bool_t sRescaleToPow2;

public:
  FBO();
  ~FBO();

  void init(int w, int h);
  void release();

  void bind();
  void unbind();

  void bind_texture();
  void unbind_texture();

}; // endclass fbo

} // endnamespace gled

#endif
