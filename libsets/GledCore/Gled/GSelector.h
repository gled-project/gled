// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef Gled_GSelector_H
#define Gled_GSelector_H

#include <Gled/GledTypes.h>
#include <Gled/GMutex.h>
#include <map>

class TSocket;

namespace gled {

struct GFdSet : public std::map<void*,Int_t>
{
  void Add(void* ud, Int_t fd);
  void Add(TSocket* s);
  void Remove(void* ud);
};

typedef GFdSet::iterator GFdSet_i;

class GSelector : public GMutex
{
public:
  enum Error_e { SE_Null=0, SE_BadFD, SE_Interrupt,
		 SE_BadArg, SE_NoMem, SE_Unknown };

  Error_e	fError;
  TString       fErrorStr;

  GFdSet	fRead;
  GFdSet	fWrite;
  GFdSet	fExcept;
  GFdSet	fReadOut;
  GFdSet	fWriteOut;
  GFdSet	fExceptOut;
  Float_t	fTimeOut;

public:
  GSelector(Init_e e=fast);
  ~GSelector();

  void Clear();

  Int_t Select();

#include "GSelector.h7"
  ClassDefNV(GSelector, 0);
}; // endclass GSelector

} // endnamespace gled

#endif
