// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include <FL/Fl_LED_Button.H>
#include <FL/Fl.H>
#include <FL/fl_draw.H>

Fl_LED_Button::Fl_LED_Button(int x, int y, int w, int h, const char* l) :
  Fl_Light_Button(x, y, w, h, l)
{
  box(FL_NO_BOX);
  selection_color(FL_RED);
}

Fl_LED_Button::~Fl_LED_Button()
{}

//==============================================================================

void Fl_LED_Button::draw_led(int X, int Y, int D)
{
  // A dark rim, the body, discs that get lighter towards the upper left, and
  // a highlight. Off, the LED keeps 30% of its color.

  const bool on = value() != 0;
  Fl_Color base = on ? selection_color() : fl_color_average(selection_color(), FL_BLACK, 0.3f);
  if (! active_r()) base = fl_inactive(base);

  fl_color(fl_color_average(base, FL_BLACK, 0.45f));
  fl_pie(X, Y, D, D, 0, 360);

  const int rim = D >= 8 ? 1 : 0;
  const int B   = D - 2 * rim;
  fl_color(base);
  fl_pie(X + rim, Y + rim, B, B, 0, 360);

  const int   steps = 4;
  const float shine = on ? 0.12f : 0.07f;
  for (int i = 1; i <= steps; ++i)
  {
    int d   = B * (steps + 1 - i) / (steps + 2);
    int off = (B - d) / 3;
    fl_color(fl_color_average(FL_WHITE, base, shine * i));
    fl_pie(X + rim + off, Y + rim + off, d, d, 0, 360);
  }

  if (D >= 6)
  {
    int hd = D / 4;
    fl_color(fl_color_average(FL_WHITE, base, on ? 0.85f : 0.45f));
    fl_pie(X + D / 4, Y + D / 5, hd, hd, 0, 360);
  }
}

void Fl_LED_Button::draw()
{
  Fl_Boxtype bt = (value() && down_box()) ? down_box() : box();
  if (bt != FL_NO_BOX) draw_box(bt, color());

  const int dx = Fl::box_dx(bt), dy = Fl::box_dy(bt);
  const int ih = h() - 2 * dy;
  int margin = ih / 8 + 1;
  int D      = ih - 2 * margin;
  if (D > w() - 2 * dx - 2 * margin) D = w() - 2 * dx - 2 * margin;
  if (D < 3) { D = ih < 3 ? ih : 3; margin = (ih - D) / 2; }

  const int lx = x() + dx + margin;
  const int ly = y() + dy + (ih - D) / 2;
  if (D > 0) draw_led(lx, ly, D);

  const int tx = lx + D + margin;
  draw_label(tx, y(), x() + w() - tx, h());
  if (Fl::focus() == this) draw_focus();
}
