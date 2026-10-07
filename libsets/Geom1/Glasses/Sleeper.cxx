// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "Sleeper.h"
#include <TSystem.h>

using namespace gled;

#include "Sleeper.c7"

void Sleeper::Operate(Operator::Arg* op_arg)
{
  gSystem->Sleep(mMSec);
}
