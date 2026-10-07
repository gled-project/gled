// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "SGlUniform.h"

using namespace gled;


// SGlUniform

//______________________________________________________________________________
//
//

//==============================================================================

SGlUniform::SGlUniform() :
  SRefCounted(),
  fIsFloat  (false),
  fType     (-1),
  fVarSize  (-1),
  fArrSize  (-1),
  fLocation (-1)
{}

SGlUniform::SGlUniform(const Text_t* name, const Text_t* defs, Bool_t is_float,
		       Int_t type, Int_t var_size, Int_t arr_size, Int_t loc) :
  SRefCounted(),

  fName     (name),
  fDefaults (defs),
  fIsFloat  (is_float),
  fType     (type),
  fVarSize  (var_size),
  fArrSize  (arr_size),
  fLocation (loc)
{}

SGlUniform::~SGlUniform()
{}

//==============================================================================

void SGlUniform::Reset(const TString& name, const TString& defs, Bool_t is_float,
		       Int_t type, Int_t var_size, Int_t arr_size, Int_t loc)
{
  fName     = name;
  fDefaults = defs;
  fIsFloat  = is_float;
  fType     = type;
  fVarSize  = var_size;
  fArrSize  = arr_size;
  fLocation = loc;
}
