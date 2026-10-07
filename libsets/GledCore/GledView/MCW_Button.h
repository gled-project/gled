// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef GledCore_MCW_Button_H
#define GledCore_MCW_Button_H

#include <Gled/GledTypes.h>
#include <Gled/GledNS.h>
#include <Eye/OptoStructs.h>

#include <FL/Fl_Button.H>

namespace gled {

class MCW_View;

class MCW_Button : public Fl_Button
{
public:
  struct Data
  {
    TString               fLabel;
    TString               fTooltip;
    GledNS::MethodInfo   *fMInfo;
    bool                  fDirectP;
    bool                  fDndP;

    Data() : fMInfo(0), fDirectP(false), fDndP(false) {}
  };

protected:
  OptoStructs::ZGlassImg *fImg;
  Data&                   fData;

public:
  MCW_Button(OptoStructs::ZGlassImg* img, Data& dt,
	     int x, int y, int w, int h, const char* t=0);

  static void FillData(GledNS::MethodInfo* mi, const char* label,  Data& d);

  virtual int handle(int ev);

}; // endclass MCW_Button

} // endnamespace gled

#endif
