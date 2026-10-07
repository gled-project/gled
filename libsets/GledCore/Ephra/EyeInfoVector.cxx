// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "EyeInfoVector.h"

using namespace gled;


// EyeInfoVector

//______________________________________________________________________________
//
//

//==============================================================================

EyeInfoVector* EyeInfoVector::CloneAndAdd(EyeInfoVector* eiv, EyeInfo* ei)
{
  const Int_t N = eiv->size();
  EyeInfoVector *v = new EyeInfoVector(N + 1);
  for (Int_t i = 0; i < N; ++i)
  {
    (*v)[i] = (*eiv)[i];
  }
  (*v)[N] = ei;
  v->IncRefCnt();
  eiv->DecRefCnt();
  return v;
}

EyeInfoVector* EyeInfoVector::CloneAndRemove(EyeInfoVector* eiv, EyeInfo* ei)
{
  const Int_t N = eiv->size();
  EyeInfoVector *v = new EyeInfoVector(N - 1);
  for (Int_t i = 0, j = 0; i < N; ++i)
  {
    if ((*eiv)[i] != ei)
      (*v)[j++] = (*eiv)[i];
  }
  v->IncRefCnt();
  eiv->DecRefCnt();
  return v;
}

//==============================================================================
