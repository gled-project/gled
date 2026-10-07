// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "Fl_PhonyEnum.h"

#include <cstdint>

using namespace gled;


void pe_cb(Fl_PhonyEnum* o, int v) {
  o->SetTrueVal(v);
  o->do_callback();
}

Fl_PhonyEnum::Fl_PhonyEnum(int x, int y, int w,int h, const char* l) :
  Fl_Choice(x,y,w,h,l), mHiIdx(-1)
{}

void Fl_PhonyEnum::AddEntry(int val, const char* label)
{
  mHiIdx++;
  mVal2Idx[val] = mHiIdx;
  add(label, 0, (Fl_Callback*)pe_cb, (void*)(intptr_t)val);
}

void Fl_PhonyEnum::Update(int v)
{
  std::map<int,int>::iterator i = mVal2Idx.find(v);
  mTrueVal = v;
  value(i->second);
}
