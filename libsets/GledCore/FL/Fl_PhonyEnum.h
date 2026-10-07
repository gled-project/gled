// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef GledCore_Fl_PhonyEnum
#define GledCore_Fl_PhonyEnum

#include <FL/Fl_Choice.H>
#include <map>

namespace gled {

class Fl_PhonyEnum : public Fl_Choice {
protected:
  std::map<int,int>	mVal2Idx;
  bool bOwnMap;
  int  mTrueVal; // X{gs}
  int  mHiIdx;

public:
  Fl_PhonyEnum(int x=0, int y=0, int w=200,int h=80, const char* l= 0);

  void AddEntry(int val, const char* label);
  void Update(int val);

#include "Fl_PhonyEnum.h7"

}; // endclass Fl_PhonyEnum

} // endnamespace gled

#endif
