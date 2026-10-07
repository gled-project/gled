// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef FL_SCROLLPACK_H
#define FL_SCROLLPACK_H

#include <FL/Fl_Pack.H>
#include <FL/Fl_Scroll.H>

#include <FL/Fl_Group.H>
#include <FL/Fl_Scrollbar.H>

class Fl_ScrollPack : public Fl_Group
{
protected:
  int ypos_;

  static void scrollbar_cb(Fl_Scrollbar*, Fl_ScrollPack*);
  void set_ypos(int y);

public:
  Fl_Scrollbar scrollbar;

  Fl_ScrollPack(int x, int y, int w, int h, const char* t=0);

  virtual void clear();
  virtual void resize(int,int,int,int);
  virtual void draw();
  virtual int  handle(int ev);
};

#endif
