// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef GledCore_MTW_ClassView_H
#define GledCore_MTW_ClassView_H

#include "MTW_View.h"
#include <FL/Fl_Pack.H>

namespace gled {

class MTW_Layout;

class MTW_ClassView : public MTW_View, public Fl_Pack
{
private:
  void _init();

protected:
  // virtual void auto_label(); use default labeling

public:
  // View created from FTW_Shell:
  MTW_ClassView(OptoStructs::ZGlassImg* img, FTW_Shell* shell);
  // Direct view for non enlightened lenses:
  MTW_ClassView(ZGlass* glass, Fl_SWM_Manager* swm_mgr);
  virtual ~MTW_ClassView();

  void BuildVerticalView();
  void BuildByLayout(MTW_Layout* layout);

  virtual int handle(int ev);
}; // endclass MTW_ClassView

} // endnamespace gled

#endif
