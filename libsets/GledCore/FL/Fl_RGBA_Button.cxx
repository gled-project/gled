// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include <FL/Fl.H>
#include <FL/Fl_RGBA_Button.H>
#include <FL/Fl_RGBA_Chooser.H>
#include <FL/fl_draw.H>

#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cstring>

namespace
{
  float clamp01(float v) { return v < 0 ? 0 : (v > 1 ? 1 : v); }

  uchar to_byte(float v) { return (uchar) (255.0f * clamp01(v) + 0.5f); }

  int hex_digit(char c)
  {
    if (c >= '0' && c <= '9') return c - '0';
    c = tolower((unsigned char) c);
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    return -1;
  }

  const char* skip_space(const char* p)
  {
    while (isspace((unsigned char) *p)) ++p;
    return p;
  }

  // Grey levels of the checkerboard behind a translucent color.
  const float k_check_light = 0.85f;
  const float k_check_dark  = 0.55f;
}

Fl_RGBA_Button::Fl_RGBA_Button(int x, int y, int w, int h, char const *t) :
  Fl_Button(x, y, w, h, t),
  rgba_chooser(0), on_change_cb(0), on_change_ud(0), th(0),
  r(0.5f), g(0.5f), b(0.5f), a(1.0f)
{
  callback((Fl_Callback*) default_cb, this);
}

Fl_RGBA_Button::~Fl_RGBA_Button()
{
  // The chooser points back to this button, so it goes now, not deferred.
  delete rgba_chooser;
}

//==============================================================================

void Fl_RGBA_Button::set_rgba(float R, float G, float B, float A)
{
  r = clamp01(R); g = clamp01(G); b = clamp01(B); a = clamp01(A);
  redraw();
}

void Fl_RGBA_Button::set_rgba(Fl_RGBA_Button* w)
{
  set_rgba(w->r, w->g, w->b, w->a);
}

void Fl_RGBA_Button::change_rgba(float R, float G, float B, float A)
{
  R = clamp01(R); G = clamp01(G); B = clamp01(B); A = clamp01(A);
  if (R == r && G == g && B == b && A == a)
    return;
  set_rgba(R, G, B, A);
  if (on_change_cb)
    on_change_cb(this, on_change_ud);
}

Fl_Color Fl_RGBA_Button::get_color()
{
  return fl_rgb_color(to_byte(r), to_byte(g), to_byte(b));
}

//==============================================================================

void Fl_RGBA_Button::copy_rgba(int clipboard)
{
  char text[64];
  int  len = snprintf(text, sizeof(text), "rgba:%5.3f/%5.3f/%5.3f/%5.3f", r, g, b, a);
  Fl::copy(text, len, clipboard);
}

bool Fl_RGBA_Button::parse_rgba(const char* text, float rgba[4])
{
  // On success stores the color, clamped, in rgba[] and returns true. The
  // formats without alpha leave rgba[3] as it was.

  if (text == 0) return false;
  const char* p = skip_space(text);
  float c[4] = { 0, 0, 0, rgba[3] };

  if (*p == '#')
  {
    ++p;
    int n = 0;
    while (hex_digit(p[n]) >= 0) ++n;
    if (n != 6 && n != 8) return false;
    for (int i = 0; i < n / 2; ++i)
      c[i] = (16 * hex_digit(p[2*i]) + hex_digit(p[2*i + 1])) / 255.0f;
    p += n;
  }
  else
  {
    int n;
    if      (strncmp(p, "rgba:", 5) == 0) { n = 4; p += 5; }
    else if (strncmp(p, "rgb:",  4) == 0) { n = 3; p += 4; }
    else return false;
    for (int i = 0; i < n; ++i)
    {
      if (i > 0)
      {
        if (*p != '/') return false;
        ++p;
      }
      char* end;
      c[i] = strtof(p, &end);
      if (end == p) return false;
      p = end;
    }
  }

  if (*skip_space(p) != 0) return false;
  for (int i = 0; i < 4; ++i) rgba[i] = clamp01(c[i]);
  return true;
}

//==============================================================================

void Fl_RGBA_Button::spawn_chooser()
{
  if (rgba_chooser == 0)
  {
    rgba_chooser = new Fl_RGBA_Chooser(this, label());
    rgba_chooser->trace_swm_manager(this);
  }
  rgba_chooser->hotspot(rgba_chooser);
  rgba_chooser->show();
}

void Fl_RGBA_Button::wipe_chooser()
{
  // Called from the chooser's own buttons, so the delete must be deferred.
  if (rgba_chooser)
  {
    rgba_chooser->hide();
    Fl::delete_widget(rgba_chooser);
    rgba_chooser = 0;
  }
}

void Fl_RGBA_Button::default_cb(Fl_RGBA_Button* w, Fl_RGBA_Button*)
{
  w->spawn_chooser();
}

//==============================================================================

void Fl_RGBA_Button::draw()
{
  Fl_Boxtype bt = value() ? (down_box() ? down_box() : fl_down(box())) : box();
  draw_box(bt, color());

  int X = x() + Fl::box_dx(bt), Y = y() + Fl::box_dy(bt);
  int W = w() - Fl::box_dw(bt), H = h() - Fl::box_dh(bt);
  if (W > 0 && H > 0)
  {
    if (a >= 1)
    {
      fl_rectf(X, Y, W, H, to_byte(r), to_byte(g), to_byte(b));
    }
    else
    {
      th = H;
      fl_draw_image(generate_achip, this, X, Y, W, H, 3);
    }
  }

  // A label drawn over the color gets a color that contrasts with it.
  if ((align() & 15) == 0 || (align() & FL_ALIGN_INSIDE))
  {
    Fl_Color lc = labelcolor();
    labelcolor(fl_contrast(lc, get_color()));
    draw_label();
    labelcolor(lc);
  }
  else
  {
    draw_label();
  }
  if (Fl::focus() == this) draw_focus();
}

void Fl_RGBA_Button::generate_achip(void* v, int X, int Y, int W, uchar* buf)
{
  // One line of the color area of a translucent color: a strip on the left
  // with the color itself, the rest the color over a checkerboard. The
  // squares are half the label size, which SWM sets to its font size, so the
  // pattern scales with the SWM cells.

  const Fl_RGBA_Button* B = (const Fl_RGBA_Button*) v;
  const int H = B->th;
  if (H <= 0) return;

  const int full_w = B->w() - Fl::box_dw(B->box());
  int strip = H < full_w / 3 ? H : full_w / 3;
  int sq    = (B->labelsize() + 1) / 2;
  if (sq < 2) sq = 2;

  const float A = clamp01(B->a), IA = 1 - A;
  const float col[3] = { B->r, B->g, B->b };
  uchar solid[3], light[3], dark[3];
  for (int i = 0; i < 3; ++i)
  {
    solid[i] = to_byte(col[i]);
    light[i] = to_byte(A * col[i] + IA * k_check_light);
    dark[i]  = to_byte(A * col[i] + IA * k_check_dark);
  }

  for (int x = X; x < X + W; ++x)
  {
    const uchar* px = x < strip ? solid : ((((x - strip) / sq + Y / sq) & 1) ? dark : light);
    *buf++ = px[0]; *buf++ = px[1]; *buf++ = px[2];
  }
}

//==============================================================================

int Fl_RGBA_Button::handle(int ev)
{
  switch (ev)
  {
    case FL_PUSH:
      if (Fl::event_button() == FL_MIDDLE_MOUSE)
      {
        Fl::paste(*this, 0);
        return 1;
      }
      break;

    case FL_RELEASE:
      if (Fl::event_button() == FL_MIDDLE_MOUSE)
        return 1;
      break;

    case FL_DRAG:
      if ((Fl::event_state() & FL_BUTTON1) && ! Fl::event_inside(this))
      {
        // Let the button pop up first, so the release does not press it.
        int ret = Fl_Button::handle(ev);
        copy_rgba(0);
        Fl::dnd();
        return ret;
      }
      break;

    case FL_DND_ENTER:
    case FL_DND_DRAG:
    case FL_DND_LEAVE:
    case FL_DND_RELEASE:
      return 1;

    case FL_PASTE:
    {
      float c[4] = { r, g, b, a };
      if (parse_rgba(Fl::event_text(), c))
        change_rgba(c[0], c[1], c[2], c[3]);
      return 1;
    }

    case FL_KEYBOARD:
      if (Fl::focus() == this && (Fl::event_state() & (FL_CTRL | FL_COMMAND)))
      {
        switch (Fl::event_key())
        {
          case 'c': copy_rgba(1);         return 1;
          case 'v': Fl::paste(*this, 1);  return 1;
        }
      }
      break;
  }

  return Fl_Button::handle(ev);
}
