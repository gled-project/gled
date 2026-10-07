// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later


#ifndef Fl_LorentzVector_H
#define Fl_LorentzVector_H

#include <FL/Fl_Group.H>
#include <FL/Fl_Value_Input.H>

namespace gled {

class Fl_LorentzVector : public Fl_Group {
protected:
  Fl_Value_Input*	mP[4];

public:
  Fl_LorentzVector(int x, int y, int w,int h, const char* l=0);

  double x() const { return mP[0]->value(); }
  double y() const { return mP[1]->value(); }
  double z() const { return mP[2]->value(); }
  double t() const { return mP[3]->value(); }

  void set(double x, double y, double z, double t) {
    if(x != mP[0]->value()) mP[0]->value(x);
    if(y != mP[1]->value()) mP[1]->value(y);
    if(z != mP[2]->value()) mP[2]->value(z);
    if(t != mP[3]->value()) mP[3]->value(t);
  }

  virtual void resize(int x, int y, int w, int h);
};

} // endnamespace gled

#endif
