// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef GTS_GTS_H
#define GTS_GTS_H


#include <gts.h>

class TString;

namespace gled
{
  class ZTrans;

  GtsSurface* MakeDefaultSurface();

  void InvertSurface(GtsSurface* s);

  void TransformSurfaceVertices(GtsSurface* s, ZTrans* t);
  void RotateSurfaceVertices(GtsSurface* s, ZTrans* t);

  void WriteSurfaceToFile(GtsSurface* s, const TString& file);
} // endnamespace gled


#endif
