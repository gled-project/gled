// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "KeyHandling.h"

using namespace gled;


using namespace KeyHandling;

KeyInfo::KeyInfo(const TString& tag, const TString& desc, AKeyCallback* foo, Int_t uid) :
  fKeyTag(tag), fKeyDesc(desc), fCallback(foo), fUserId(uid), fIndex(-1), fDownCount(0)
{}

KeyInfo::~KeyInfo()
{
  delete fCallback;
}
