// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "SSocket.h"

using namespace gled;


// SSocket

//______________________________________________________________________________
//
// Wrapper over TSocket avoiding double invocation of Close() from reader and
// writer threads.

//==============================================================================

void SSocket::Close(Option_t *opt)
{
  GMutexHolder _lck(mMutex);
  if (!mClosedDown)
  {
    TSocket::Close(opt);
    mClosedDown = true;
  }
}
