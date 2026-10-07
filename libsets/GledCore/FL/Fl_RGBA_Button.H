// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef Fl_RGBA_Button_H
#define Fl_RGBA_Button_H

#include <FL/Fl_Button.H>

class Fl_RGBA_Chooser;

// A button that shows an RGBA color, components are floats in [0, 1].
//
// Pressing the button opens an Fl_RGBA_Chooser for it. Dragging the mouse out
// of the button copies the color as "rgba:r/g/b/a" into the selection buffer
// and starts drag-and-drop. The color is set by dropping, or by middle-click
// pasting, a string "rgba:r/g/b/a", "rgb:r/g/b", "#rrggbb" or "#rrggbbaa".
// With keyboard focus, ctrl-c copies the color to the clipboard and ctrl-v
// pastes it from there.
//
// on_change() is called when the user changes the color through the chooser,
// a paste or a drop, and only if the color differs. set_rgba() does not call
// it; it is for updating the button from the program.

class Fl_RGBA_Button : public Fl_Button
{
protected:
  static void	generate_achip(void*, int, int, int, uchar*);
  Fl_RGBA_Chooser* rgba_chooser;
  Fl_Callback*	on_change_cb;
  void*		on_change_ud;
  int		th;     // height of the color area, for generate_achip()

public:
  float		r, g, b, a;

  Fl_RGBA_Button(int x, int y, int w, int h, char const *t=0);
  ~Fl_RGBA_Button();

  void set_rgba(float, float, float, float a=1);
  void set_rgba(Fl_RGBA_Button* w);
  void change_rgba(float, float, float, float);
  Fl_Color get_color();

  void copy_rgba(int clipboard);
  static bool parse_rgba(const char* text, float rgba[4]);

  void spawn_chooser();
  void wipe_chooser();
  void on_change(Fl_Callback* cb, void* ud) { on_change_cb=cb; on_change_ud = ud; }
  Fl_Callback* on_change() { return on_change_cb; }
  void* on_change_data() { return on_change_ud; }
  static void default_cb(Fl_RGBA_Button* w, Fl_RGBA_Button* ud);

  virtual void draw();
  virtual int handle(int ev);
};

#endif
