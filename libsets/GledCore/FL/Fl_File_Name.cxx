// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include <FL/Fl_File_Name.H>

#include <FL/Fl_File_Chooser.H>

int Fl_File_Name::handle(int ev)
{
  if(ev==FL_PUSH && Fl::event_button() == 3) return 1;
  if(ev==FL_RELEASE && Fl::event_button() == 3) {
    int relative = _abs ? 0 : 1;
    char* file   = _dir ?
      fl_dir_chooser (label(), value(), relative):
      fl_file_chooser(label(), _pat, value(), relative);
    if(file) {
      value(file);
      do_callback();
      redraw();
    }
    return 1;
  }
  return Fl_Input::handle(ev);
}
