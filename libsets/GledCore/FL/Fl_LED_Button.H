// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef Fl_LED_Button_H
#define Fl_LED_Button_H

#include <FL/Fl_Light_Button.H>

// A light button drawn as a round LED followed by the label. The LED is lit in
// selection_color() when value() is on; when off it shows a dim version of the
// same color. The LED is sized from the height of the button, so it scales
// with SWM. For a status display call set_output(): FLTK then sends the
// widget no events and the program alone sets value().

class Fl_LED_Button : public Fl_Light_Button
{
protected:
  void draw_led(int x, int y, int d);

public:
  Fl_LED_Button(int x, int y, int w, int h, const char* l=0);
  ~Fl_LED_Button();

  virtual void draw();
};

#endif
