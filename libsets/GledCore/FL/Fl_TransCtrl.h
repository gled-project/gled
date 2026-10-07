// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later


#ifndef Fl_TransCtrl_H
#define Fl_TransCtrl_H

#include <FL/Fl_Group.H>
#include <FL/Fl_Value_Input.H>

namespace gled {

class Fl_TransPosCtrl : public Fl_Group
{
protected:
  Fl_Value_Input*	mP[3];

public:
  Fl_TransPosCtrl(int x, int y, int w,int h, const char* l=0);

  double x() const { return mP[0]->value(); }
  double y() const { return mP[1]->value(); }
  double z() const { return mP[2]->value(); }

  void pos(double x, double y, double z);

  virtual void resize(int x, int y, int w, int h);
};

//------------------------------------------------------------------------------

class Fl_TransRotCtrl : public Fl_Group
{
protected:
  Fl_Value_Input*	mP[3];
  bool                  mLock;

  static const double sD2R, sR2D;

public:
  Fl_TransRotCtrl(int x, int y, int w,int h, const char* l=0);

  double phi()   const { return mP[0]->value() * sD2R; }
  double theta() const { return mP[1]->value() * sD2R; }
  double eta()   const { return mP[2]->value() * sD2R; }

  void   rot(double x, double y, double z);

  double mungl(double x) const;

  virtual void resize(int x, int y, int w, int h);
};

} // endnamespace gled

#endif
