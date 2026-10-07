// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include <FL/Fl_RGBA_Chooser.H>
#include <FL/Fl_RGBA_Button.H>
#include <FL/Fl.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Return_Button.H>
#include <FL/Fl_Value_Input.H>
#include <FL/fl_draw.H>

#include <cmath>
#include <cstdio>

namespace
{
  float clamp01(float v) { return v < 0 ? 0 : (v > 1 ? 1 : v); }

  uchar to_byte(float v) { return (uchar) (255.0f * clamp01(v) + 0.5f); }

  // Grey levels of the checkerboard behind a translucent color, as in
  // Fl_RGBA_Button.
  const float k_check_light = 0.85f;
  const float k_check_dark  = 0.55f;

  // Half the label size, which SWM sets to its font size: the unit for
  // markers, thumbs and checker squares.
  int half_font(const Fl_Widget* w)
  {
    int s = (w->labelsize() + 1) / 2;
    return s < 2 ? 2 : s;
  }

  // A value input that can show its value as 0xNN. Fl_Value_Input parses
  // input with strtol(.., 0) when the step is integral, so 0x.. reads back.
  class HexValueInput : public Fl_Value_Input
  {
  public:
    bool hex;
    HexValueInput(int x, int y, int w, int h) : Fl_Value_Input(x, y, w, h), hex(false) {}
    // Reformats the text; value() does it only when the value changes.
    void refresh() { char buf[64]; format(buf); input.value(buf); }

    // The mouse wheel steps the value: 1 for integer steps and for the hue
    // in degrees, else 0.01; ten times that with shift.
    int handle(int ev)
    {
      if (ev == FL_MOUSEWHEEL)
      {
        if (! Fl::event_inside(this) || Fl::event_dy() == 0) return 0;
        double inc = (step() >= 1 || maximum() > 1) ? 1 : 0.01;
        if (Fl::event_state() & FL_SHIFT) inc *= 10;
        double v = value() - inc * Fl::event_dy();
        if (v < minimum()) v = minimum(); else if (v > maximum()) v = maximum();
        if (v != value()) { value(v); do_callback(); }
        return 1;
      }
      return Fl_Value_Input::handle(ev);
    }
    int format(char* buf)
    {
      if (hex) return sprintf(buf, "0x%02X", (int) (value() + 0.5));
      return Fl_Value_Input::format(buf);
    }
  };
}

//==============================================================================
// Hue / saturation field: hue across, saturation from 1 at the top to 0 at
// the bottom, drawn at full value.
//==============================================================================

class Fl_RGBA_Chooser::Field : public Fl_Widget
{
  Fl_RGBA_Chooser* C;

  static void line(void* v, int X, int Y, int W, uchar* buf)
  {
    const Field* f = (const Field*) v;
    const int w = f->w() - Fl::box_dw(f->box()), h = f->h() - Fl::box_dh(f->box());
    const float s = h > 1 ? 1.0f - float(Y) / (h - 1) : 1.0f;
    for (int x = X; x < X + W; ++x)
    {
      float r, g, b;
      hsv_to_rgb(w > 1 ? 360.0f * x / (w - 1) : 0, s, 1, r, g, b);
      *buf++ = to_byte(r); *buf++ = to_byte(g); *buf++ = to_byte(b);
    }
  }

public:
  Field(int x, int y, int w, int h, Fl_RGBA_Chooser* c) : Fl_Widget(x, y, w, h), C(c)
  { box(FL_DOWN_FRAME); }

  void draw()
  {
    draw_box();
    const int X = x() + Fl::box_dx(box()), Y = y() + Fl::box_dy(box());
    const int W = w() - Fl::box_dw(box()), H = h() - Fl::box_dh(box());
    if (W <= 0 || H <= 0) return;
    fl_push_clip(X, Y, W, H);
    fl_draw_image(line, this, X, Y, W, H, 3);
    const int px = X + (int) (C->hue() / 360.0f * (W - 1) + 0.5f);
    const int py = Y + (int) ((1 - C->saturation()) * (H - 1) + 0.5f);
    const int m  = half_font(this) / 2 + 2;
    fl_color(FL_BLACK); fl_rect(px - m, py - m, 2*m + 1, 2*m + 1);
    fl_color(FL_WHITE); fl_rect(px - m + 1, py - m + 1, 2*m - 1, 2*m - 1);
    fl_pop_clip();
  }

  int handle(int ev)
  {
    switch (ev)
    {
      case FL_PUSH:
      case FL_DRAG:
      {
        const int X = x() + Fl::box_dx(box()), Y = y() + Fl::box_dy(box());
        const int W = w() - Fl::box_dw(box()), H = h() - Fl::box_dh(box());
        float hue = W > 1 ? 360.0f * (Fl::event_x() - X) / (W - 1) : 0;
        if (hue < 0) hue = 0; else if (hue > 359.99f) hue = 359.99f;
        const float s = H > 1 ? 1.0f - float(Fl::event_y() - Y) / (H - 1) : 1;
        C->set_hsva(hue, clamp01(s), C->value(), C->a(), this);
        return 1;
      }
      case FL_RELEASE:
      case FL_ENTER:
      case FL_LEAVE:
        return 1;
      case FL_MOUSEWHEEL:
        // The wheel changes the value, as on the value slider.
        if (! Fl::event_inside(this)) return 0;
        C->set_hsva(C->hue(), C->saturation(), clamp01(C->value() - 0.02f * Fl::event_dy()), C->a(), this);
        return 1;
    }
    return Fl_Widget::handle(ev);
  }
};

//==============================================================================
// Vertical sliders: 1 at the top, 0 at the bottom, with a thumb. The mouse
// wheel steps by 2%.
//==============================================================================

class Fl_RGBA_Chooser::Bar : public Fl_Widget
{
protected:
  Fl_RGBA_Chooser* C;

  virtual float get() const = 0;
  virtual void  set(float v) = 0;
  // Color at height t (1 top, 0 bottom) and image position x, y.
  virtual void  sample(float t, int x, int y, uchar* out) const = 0;

  int thumb() const { int t = labelsize() / 2; return t < 4 ? 4 : t; }

  static void line(void* v, int X, int Y, int W, uchar* buf)
  {
    const Bar* b = (const Bar*) v;
    const int h = b->h() - Fl::box_dh(b->box());
    const float t = h > 1 ? 1.0f - float(Y) / (h - 1) : 1.0f;
    for (int x = X; x < X + W; ++x, buf += 3)
      b->sample(t, x, Y, buf);
  }

public:
  Bar(int x, int y, int w, int h, Fl_RGBA_Chooser* c) : Fl_Widget(x, y, w, h), C(c)
  { box(FL_DOWN_FRAME); }

  void draw()
  {
    draw_box();
    const int X = x() + Fl::box_dx(box()), Y = y() + Fl::box_dy(box());
    const int W = w() - Fl::box_dw(box()), H = h() - Fl::box_dh(box());
    if (W <= 0 || H <= 0) return;
    fl_draw_image(line, this, X, Y, W, H, 3);
    int t = thumb(); if (t > H) t = H;
    const int ty = Y + (int) ((1 - get()) * (H - t) + 0.5f);
    draw_box(FL_UP_BOX, X, ty, W, t, FL_GRAY);
  }

  int handle(int ev)
  {
    switch (ev)
    {
      case FL_PUSH:
      case FL_DRAG:
      {
        const int Y = y() + Fl::box_dy(box()), H = h() - Fl::box_dh(box());
        const int t = thumb();
        set(clamp01(H > t ? 1.0f - float(Fl::event_y() - Y - t / 2) / (H - t) : 0));
        return 1;
      }
      case FL_RELEASE:
      case FL_ENTER:
      case FL_LEAVE:
        return 1;
      case FL_MOUSEWHEEL:
        // FLTK offers the wheel to the other children as well; take it only
        // when the mouse is here.
        if (! Fl::event_inside(this)) return 0;
        set(clamp01(get() - 0.02f * Fl::event_dy()));
        return 1;
    }
    return Fl_Widget::handle(ev);
  }
};

class Fl_RGBA_Chooser::VBar : public Fl_RGBA_Chooser::Bar
{
protected:
  float get() const { return C->value(); }
  void  set(float v) { C->set_hsva(C->hue(), C->saturation(), v, C->a(), this); }
  void  sample(float t, int, int, uchar* out) const
  {
    float r, g, b;
    hsv_to_rgb(C->hue(), C->saturation(), t, r, g, b);
    out[0] = to_byte(r); out[1] = to_byte(g); out[2] = to_byte(b);
  }
public:
  VBar(int x, int y, int w, int h, Fl_RGBA_Chooser* c) : Bar(x, y, w, h, c) {}
};

class Fl_RGBA_Chooser::ABar : public Fl_RGBA_Chooser::Bar
{
protected:
  float get() const { return C->a(); }
  void  set(float v) { C->set_rgba(C->r(), C->g(), C->b(), v, this); }
  void  sample(float t, int x, int y, uchar* out) const
  {
    const int   sq = half_font(this);
    const float bg = ((x / sq + y / sq) & 1) ? k_check_dark : k_check_light;
    out[0] = to_byte(t * C->r() + (1 - t) * bg);
    out[1] = to_byte(t * C->g() + (1 - t) * bg);
    out[2] = to_byte(t * C->b() + (1 - t) * bg);
  }
public:
  ABar(int x, int y, int w, int h, Fl_RGBA_Chooser* c) : Bar(x, y, w, h, c) {}
};

//==============================================================================
// Fl_RGBA_Chooser
//==============================================================================

Fl_RGBA_Chooser::Fl_RGBA_Chooser(Fl_RGBA_Button* master, const char* label) :
  Fl_Window(30, 8, label), Fl_SWM_Client(), m_master(master),
  m_r(master->r), m_g(master->g), m_b(master->b), m_a(master->a),
  m_h(0), m_s(0), m_v(0)
{
  m_orig[0] = m_r; m_orig[1] = m_g; m_orig[2] = m_b; m_orig[3] = m_a;

  m_field = new Field( 0, 0, 18, 7, this);
  m_vbar  = new VBar (18, 0,  2, 7, this);
  m_abar  = new ABar (20, 0,  2, 7, this);
  m_vbar->tooltip("value");
  m_abar->tooltip("alpha");

  m_mode = new Fl_Choice(22, 0, 8, 1);
  m_mode->add("rgb|byte|hex|hsv");
  m_mode->value(M_RGB);
  m_mode->callback(s_mode_cb, this);

  for (int i = 0; i < 4; ++i)
  {
    m_in[i] = new HexValueInput(22, 1 + i, 8, 1);
    m_in[i]->callback(s_input_cb, this);
  }

  m_text = new Fl_Input(22, 5, 8, 1);
  m_text->when(FL_WHEN_CHANGED);
  m_text->callback(s_text_cb, this);
  m_text->tooltip("#rrggbb, #rrggbbaa, rgb:r/g/b or rgba:r/g/b/a");

  m_ok = new Fl_Return_Button(0, 7, 8, 1, "OK");
  m_ok->callback(s_ok_cb, this);

  m_set = new Fl_RGBA_Button(8, 7, 7, 1, "Set");
  m_set->callback(s_set_cb, this);
  m_set->on_change(s_drop_cb, this);
  m_set->tooltip("apply; drop a color here to load it");

  m_undo = new Fl_RGBA_Button(15, 7, 7, 1, "Undo");
  m_undo->set_rgba(m_r, m_g, m_b, m_a);
  m_undo->callback(s_undo_cb, this);
  m_undo->tooltip("back to the color at opening");

  m_close = new Fl_Button(22, 7, 8, 1, "Close");
  m_close->callback(s_close_cb, this);

  end();
  resizable(m_field);
  swm_size_range = new SWM_Size_Range(30, 8, 30*4, 8*4);
  callback(s_close_cb, this);   // Esc and the window manager's close

  configure_inputs();
  set_rgba(m_r, m_g, m_b, m_a);
}

//------------------------------------------------------------------------------

void Fl_RGBA_Chooser::set_rgba(float r, float g, float b, float a, Fl_Widget* source)
{
  m_r = clamp01(r); m_g = clamp01(g); m_b = clamp01(b); m_a = clamp01(a);
  float h, s, v;
  rgb_to_hsv(m_r, m_g, m_b, h, s, v);
  // Keep the hue of greys and the hue and saturation of black, so that the
  // field does not jump while the value slider passes through them.
  if (v > 0)
  {
    if (s > 0) m_h = h;
    m_s = s;
  }
  m_v = v;
  update(source);
}

void Fl_RGBA_Chooser::set_hsva(float h, float s, float v, float a, Fl_Widget* source)
{
  h = std::fmod(h, 360.0f);
  m_h = h < 0 ? h + 360.0f : h;
  m_s = clamp01(s); m_v = clamp01(v); m_a = clamp01(a);
  hsv_to_rgb(m_h, m_s, m_v, m_r, m_g, m_b);
  update(source);
}

void Fl_RGBA_Chooser::update(Fl_Widget* source)
{
  // Brings all widgets to the current color, except the input the user is
  // typing into.

  const int md = mode();
  for (int i = 0; i < 4; ++i)
  {
    if (m_in[i] == source) continue;
    float c;
    if (md == M_HSV) c = i == 0 ? m_h : (i == 1 ? m_s : (i == 2 ? m_v : m_a));
    else             c = i == 0 ? m_r : (i == 1 ? m_g : (i == 2 ? m_b : m_a));
    if (md == M_Byte || md == M_Hex) c = (int) (255.0f * c + 0.5f);
    m_in[i]->value(c);
  }
  if (m_text != source)
  {
    char buf[16];
    snprintf(buf, sizeof(buf), "#%02x%02x%02x%02x",
             to_byte(m_r), to_byte(m_g), to_byte(m_b), to_byte(m_a));
    m_text->value(buf);
  }
  m_set->set_rgba(m_r, m_g, m_b, m_a);
  m_field->redraw();
  m_vbar->redraw();
  m_abar->redraw();
}

void Fl_RGBA_Chooser::configure_inputs()
{
  static const char* const rgb_tips[4] = { "red",   "green",      "blue",  "alpha" };
  static const char* const hsv_tips[4] = { "hue, degrees", "saturation", "value", "alpha" };

  const int md = mode();
  for (int i = 0; i < 4; ++i)
  {
    HexValueInput* in = (HexValueInput*) m_in[i];
    in->hex = md == M_Hex;
    if (md == M_Byte || md == M_Hex)  { in->bounds(0, 255); in->step(1); }
    else if (md == M_HSV && i == 0)   { in->bounds(0, 360); in->step(0.1); }
    else                              { in->bounds(0, 1);   in->step(0.001); }
    in->tooltip(md == M_HSV ? hsv_tips[i] : rgb_tips[i]);
  }
  update(0);
  for (int i = 0; i < 4; ++i) ((HexValueInput*) m_in[i])->refresh();
}

int Fl_RGBA_Chooser::mode() const
{
  return m_mode->value();
}

void Fl_RGBA_Chooser::mode(int m)
{
  m_mode->value(m);
  configure_inputs();
}

//------------------------------------------------------------------------------

void Fl_RGBA_Chooser::apply()
{
  m_master->change_rgba(m_r, m_g, m_b, m_a);
}

void Fl_RGBA_Chooser::apply_and_close()
{
  apply();
  close();
}

void Fl_RGBA_Chooser::revert()
{
  set_rgba(m_orig[0], m_orig[1], m_orig[2], m_orig[3]);
}

void Fl_RGBA_Chooser::close()
{
  // The button hides and deletes this window, deferred.
  m_master->wipe_chooser();
}

void Fl_RGBA_Chooser::trace_swm_manager(Fl_Widget* w)
{
  Fl_SWM_Client::trace_swm_manager(w);
  if (swm_manager == 0)
  {
    // No manager: cells of the width of 'X' plus two, 18 pixels high, font 12.
    fl_font(FL_HELVETICA, 12);
    Fl_SWM_Manager::resize_group(this, 1, 1, (int) fl_width('X') + 2, 18, 12);
  }
}

//------------------------------------------------------------------------------

void Fl_RGBA_Chooser::s_mode_cb(Fl_Widget*, void* c)
{ ((Fl_RGBA_Chooser*) c)->configure_inputs(); }

void Fl_RGBA_Chooser::s_input_cb(Fl_Widget* w, void* cc)
{
  Fl_RGBA_Chooser* c = (Fl_RGBA_Chooser*) cc;
  float v[4];
  for (int i = 0; i < 4; ++i) v[i] = (float) c->m_in[i]->value();
  switch (c->mode())
  {
    case M_HSV:
      c->set_hsva(v[0], v[1], v[2], v[3], w);
      break;
    case M_Byte:
    case M_Hex:
      c->set_rgba(v[0] / 255, v[1] / 255, v[2] / 255, v[3] / 255, w);
      break;
    default:
      c->set_rgba(v[0], v[1], v[2], v[3], w);
      break;
  }
}

void Fl_RGBA_Chooser::s_text_cb(Fl_Widget*, void* cc)
{
  Fl_RGBA_Chooser* c = (Fl_RGBA_Chooser*) cc;
  float rgba[4] = { c->m_r, c->m_g, c->m_b, c->m_a };
  if (Fl_RGBA_Button::parse_rgba(c->m_text->value(), rgba))
    c->set_rgba(rgba[0], rgba[1], rgba[2], rgba[3], c->m_text);
}

void Fl_RGBA_Chooser::s_ok_cb(Fl_Widget*, void* c)
{ ((Fl_RGBA_Chooser*) c)->apply_and_close(); }

void Fl_RGBA_Chooser::s_set_cb(Fl_Widget*, void* c)
{ ((Fl_RGBA_Chooser*) c)->apply(); }

void Fl_RGBA_Chooser::s_drop_cb(Fl_Widget*, void* cc)
{
  Fl_RGBA_Chooser* c = (Fl_RGBA_Chooser*) cc;
  c->set_rgba(c->m_set->r, c->m_set->g, c->m_set->b, c->m_set->a, c->m_set);
}

void Fl_RGBA_Chooser::s_undo_cb(Fl_Widget*, void* c)
{ ((Fl_RGBA_Chooser*) c)->revert(); }

void Fl_RGBA_Chooser::s_close_cb(Fl_Widget*, void* c)
{ ((Fl_RGBA_Chooser*) c)->close(); }

//==============================================================================

void Fl_RGBA_Chooser::hsv_to_rgb(float h, float s, float v, float& r, float& g, float& b)
{
  h = std::fmod(h, 360.0f);
  if (h < 0) h += 360.0f;
  const float c  = v * s;
  const float hp = h / 60.0f;
  const float x  = c * (1.0f - std::fabs(std::fmod(hp, 2.0f) - 1.0f));
  float r1 = 0, g1 = 0, b1 = 0;
  switch ((int) hp)
  {
    case 0:  r1 = c; g1 = x; break;
    case 1:  r1 = x; g1 = c; break;
    case 2:  g1 = c; b1 = x; break;
    case 3:  g1 = x; b1 = c; break;
    case 4:  r1 = x; b1 = c; break;
    default: r1 = c; b1 = x; break;
  }
  const float m = v - c;
  r = r1 + m; g = g1 + m; b = b1 + m;
}

void Fl_RGBA_Chooser::rgb_to_hsv(float r, float g, float b, float& h, float& s, float& v)
{
  const float mx = r > g ? (r > b ? r : b) : (g > b ? g : b);
  const float mn = r < g ? (r < b ? r : b) : (g < b ? g : b);
  const float d  = mx - mn;
  v = mx;
  s = mx > 0 ? d / mx : 0;
  if (d <= 0)        h = 0;
  else if (mx == r)  h = 60.0f * std::fmod((g - b) / d, 6.0f);
  else if (mx == g)  h = 60.0f * ((b - r) / d + 2.0f);
  else               h = 60.0f * ((r - g) / d + 4.0f);
  if (h < 0) h += 360.0f;
}
