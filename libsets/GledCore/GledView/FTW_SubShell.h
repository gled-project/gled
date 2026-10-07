// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef GledCore_FTW_SubShell_H
#define GledCore_FTW_SubShell_H

#include <TString.h>
#include "FTW_ShellClient.h"

class Fl_Window;
class Fl_Group;
class Fl_Widget;

namespace gled {

class FTW_SubShell : public FTW_ShellClient
{
protected:
  Fl_Window*    mWindow;
  Fl_Widget*    mContents;

  TString       mWindowLabel;

public:
  FTW_SubShell(FTW_Shell* s, Fl_Window* w, Fl_Widget* c);
  virtual ~FTW_SubShell();

  Fl_Window* GetWindow() { return mWindow; }

  virtual void label_window(const char* l=0);

  virtual void dock(Fl_Group* g);
  virtual void undock();
}; // endclass FTW_SubShell

} // endnamespace gled

#endif
