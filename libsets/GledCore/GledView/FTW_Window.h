// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef GledCore_FTW_Window_H
#define GledCore_FTW_Window_H

#include <FL/Fl_Double_Window.H>
#include <FL/Fl_SWM.H>

namespace gled {

class FTW_Window : public Fl_Double_Window, public Fl_SWM_Client
{
public:
  FTW_Window(int w, int h, const char* t=0);
  FTW_Window(int x, int y, int w, int h, const char *t=0);
};

} // endnamespace gled

#endif
