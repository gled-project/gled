// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef GledCore_SEvTaskState_GL_RNR_H
#define GledCore_SEvTaskState_GL_RNR_H

#include <Stones/SEvTaskState.h>

namespace gled {
class ZColor;

class SEvTaskState_GL_Rnr {

public:
  static void RenderHisto(const SEvTaskState& ts, ZColor* cols);
  static void RenderBar(const SEvTaskState& ts, ZColor* cols);

}; // endclass SEvTaskState_GL_Rnr

} // endnamespace gled

#endif
