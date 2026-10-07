// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef GledCore_EyeInfoVector_H
#define GledCore_EyeInfoVector_H

#include <Gled/GledTypes.h>
// #include <Gled/GSpinLock.h>
#include <Gled/GMutex.h>

#include <vector>

namespace gled {

class EyeInfo;

class EyeInfoVector : public std::vector<EyeInfo*>
{
protected:
  GMutex     mLock;
  Int_t      mRefCnt;

public:
  EyeInfoVector(Int_t s=0) : std::vector<EyeInfo*>(s), mRefCnt(0) {}
  ~EyeInfoVector() {}

  void IncRefCnt() { ++mRefCnt; }
  void DecRefCnt() { mLock.Lock(); if (--mRefCnt == 0) delete this; else mLock.Unlock(); }

  static EyeInfoVector* CloneAndAdd(EyeInfoVector* eiv, EyeInfo* ei);
  static EyeInfoVector* CloneAndRemove(EyeInfoVector* eiv, EyeInfo* ei);

#include "EyeInfoVector.h7"
  ClassDefNV(EyeInfoVector, 0);
}; // endclass EyeInfoVector

} // endnamespace gled

#endif
