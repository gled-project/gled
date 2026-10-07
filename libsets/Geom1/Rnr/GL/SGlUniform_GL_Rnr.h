// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef Geom1_SGlUniform_GL_RNR_H
#define Geom1_SGlUniform_GL_RNR_H

#include <Stones/SGlUniform.h>

#include <GL/glew.h>

class TPMERegexp;

namespace gled {

class RnrDriver;

class SGlUniform_GL_Rnr
{
protected:
  SGlUniform            *mUni;
  SGlUniform::DataPtr_u  mData;

  void alloc();
  void dealloc();
  void realloc();

  static TPMERegexp *s_valuesep_re;

public:
  SGlUniform_GL_Rnr(SGlUniform* u);
  SGlUniform_GL_Rnr(const SGlUniform_GL_Rnr &a);
  virtual ~SGlUniform_GL_Rnr();

  void parse(const TString& vals);
  void parse_defaults();

  void apply();

  static const char* unitype_to_name(GLenum t);
  static Int_t       unitype_to_size(GLenum t);
  static Bool_t      unitype_is_float(GLenum t);

}; // endclass SGlUniform_GL_Rnr

} // endnamespace gled

#endif
