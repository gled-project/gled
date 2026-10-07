// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "FTW_Window.h"

using namespace gled;


FTW_Window::FTW_Window(int w, int h, const char* t) :
  Fl_Double_Window(w,h,t),
  Fl_SWM_Client()
{}

FTW_Window::FTW_Window(int x, int y, int w, int h, const char* t) :
  Fl_Double_Window(x,y,w,h,t),
  Fl_SWM_Client()
{}
