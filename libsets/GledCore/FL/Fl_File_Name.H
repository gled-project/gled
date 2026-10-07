// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef Fl_File_Name_H
#define Fl_File_Name_H

#include <FL/Fl.H>
#include <FL/Fl_Input.H>

class Fl_File_Name : public Fl_Input
{
protected:
  const char* _pat;
  bool        _dir;
  bool        _abs;

public:
  Fl_File_Name(int x, int y, int w, int h, const char *label = 0) :
    Fl_Input(x,y,w,h,label), _pat(0), _dir(false), _abs(false)
  { value(""); }

  virtual int handle(int ev);

  const char* pattern() const { return _pat; }
  void pattern(const char* p) { _pat = p; }

  bool directory_p()          { return _dir; }
  void directory_p(bool d)    { _dir = d; }

  bool absolute_p()           { return _abs; }
  void absolute_p(bool a)     { _abs = a; }

}; // endclass Fl_File_Name

#endif
