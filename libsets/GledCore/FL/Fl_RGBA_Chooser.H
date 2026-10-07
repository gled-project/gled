// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef Fl_RGBA_Chooser_H
#define Fl_RGBA_Chooser_H

#include <FL/Fl_Window.H>
#include <FL/Fl_SWM.H>

class Fl_RGBA_Button;
class Fl_Choice;
class Fl_Input;
class Fl_Value_Input;
class Fl_Return_Button;
class Fl_Button;

// Color chooser window for an Fl_RGBA_Button, built in SWM cells (30 x 8).
//
// Left: a hue / saturation field, a value slider and an alpha slider. Right:
// the input mode (rgb 0-1, byte 0-255, hex 0x00-0xFF, hsv with hue in
// degrees), four inputs for the components and a text field that takes any
// color Fl_RGBA_Button::parse_rgba() understands, e.g. #rrggbbaa. Bottom: OK
// (also Enter) applies the color and closes, the Set swatch shows the new
// color and applies it, the Undo swatch shows the color the chooser was opened
// with and goes back to it, Close (also Esc) closes without applying.
// Dropping a color onto the Set swatch loads it into the chooser.

class Fl_RGBA_Chooser : public Fl_Window, public Fl_SWM_Client
{
public:
  enum Mode_e { M_RGB, M_Byte, M_Hex, M_HSV };

  class Field;    // hue / saturation field
  class Bar;      // vertical slider base
  class VBar;     // value slider
  class ABar;     // alpha slider

private:
  Fl_RGBA_Button   *m_master;
  float             m_r, m_g, m_b, m_a;
  float             m_h, m_s, m_v;        // hue in degrees [0, 360)
  float             m_orig[4];

  Field            *m_field;
  VBar             *m_vbar;
  ABar             *m_abar;
  Fl_Choice        *m_mode;
  Fl_Value_Input   *m_in[4];
  Fl_Input         *m_text;
  Fl_Return_Button *m_ok;
  Fl_RGBA_Button   *m_set;
  Fl_RGBA_Button   *m_undo;
  Fl_Button        *m_close;

  void configure_inputs();
  void update(Fl_Widget* source);

  static void s_mode_cb (Fl_Widget*, void*);
  static void s_input_cb(Fl_Widget*, void*);
  static void s_text_cb (Fl_Widget*, void*);
  static void s_ok_cb   (Fl_Widget*, void*);
  static void s_set_cb  (Fl_Widget*, void*);
  static void s_drop_cb (Fl_Widget*, void*);
  static void s_undo_cb (Fl_Widget*, void*);
  static void s_close_cb(Fl_Widget*, void*);

public:
  Fl_RGBA_Chooser(Fl_RGBA_Button* master, const char* label=0);

  void set_rgba(float r, float g, float b, float a, Fl_Widget* source=0);
  void set_hsva(float h, float s, float v, float a, Fl_Widget* source=0);

  float r() const { return m_r; }
  float g() const { return m_g; }
  float b() const { return m_b; }
  float a() const { return m_a; }
  float hue()        const { return m_h; }
  float saturation() const { return m_s; }
  float value()      const { return m_v; }

  int  mode() const;
  void mode(int m);

  void apply();
  void apply_and_close();
  void revert();
  void close();

  virtual void trace_swm_manager(Fl_Widget* w);

  static void hsv_to_rgb(float h, float s, float v, float& r, float& g, float& b);
  static void rgb_to_hsv(float r, float g, float b, float& h, float& s, float& v);
};

#endif
