// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later


#ifndef Fl_Vector3_H
#define Fl_Vector3_H

#include <FL/Fl_Group.H>
#include <FL/Fl_Value_Input.H>

namespace gled {

class Fl_Vector3 : public Fl_Group {
protected:
  Fl_Value_Input*	mP[3];

public:
  Fl_Vector3(int x, int y, int w,int h, const char* l=0);

  double x() const { return mP[0]->value(); }
  double y() const { return mP[1]->value(); }
  double z() const { return mP[2]->value(); }

  void set(double x, double y, double z) {
    if(x != mP[0]->value()) mP[0]->value(x);
    if(y != mP[1]->value()) mP[1]->value(y);
    if(z != mP[2]->value()) mP[2]->value(z);
  }

  virtual void resize(int x, int y, int w, int h);
};

} // endnamespace gled

#endif
