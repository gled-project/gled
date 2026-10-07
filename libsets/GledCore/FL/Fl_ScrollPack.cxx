// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include <FL/Fl_ScrollPack.H>
#include <FL/Fl.H>
#include <FL/fl_draw.H>

#include <stdio.h>

// scrollbar is the first child ... keep it there.

/**************************************************************************/

void Fl_ScrollPack::scrollbar_cb(Fl_Scrollbar* o, Fl_ScrollPack* sp)
{ sp->set_ypos(int(o->value())); }

void Fl_ScrollPack::set_ypos(int y)
{
  ypos_ = y;
  redraw();
}

/**************************************************************************/

Fl_ScrollPack::Fl_ScrollPack(int x, int y, int w, int h, const char* t) :
  Fl_Group(x, y, w, h, t),
  scrollbar(x+w-2, y, 2, h)
{
  ypos_   = 0;
  resizable(0);
  
  scrollbar.callback((Fl_Callback*)scrollbar_cb, this);
}

/**************************************************************************/

void Fl_ScrollPack::clear() {
  for (int i=children() - 1; i > 0; i--) {
    Fl_Widget* c = child(i);
    remove(c);
    delete c;
  }
}

void Fl_ScrollPack::resize(int X, int Y, int W, int H)
{
  scrollbar.resize(X+W-scrollbar.w(), Y, scrollbar.w(), H);
  Fl_Widget::resize(X,Y,W,H);
}

void Fl_ScrollPack::draw()
{
  int vish = 0;
  for(int i=1; i<=children()-1; ++i) {
    Fl_Widget* c = child(i);
    if(c->visible()) vish += c->h();
  }
  scrollbar.value(ypos_, h(), 0, vish);

  fl_push_clip(x(), y(), w()-scrollbar.w(), h());
  draw_box();
  int  ycur = 0;
  bool done = false;
  for(int i=1; i<=children()-1; ++i) {
    Fl_Widget* c = child(i);
    if(c->visible()) {
      if(done) {
	c->Fl_Widget::resize(c->x(), -32000, c->w(), c->h());
      } else {
	if(ycur + c->h() > ypos_) {
	  c->Fl_Widget::resize(c->x(), y()+ycur-ypos_, c->w(), c->h());
	  draw_child(*c);
	} else {
	  c->Fl_Widget::resize(c->x(), -32000, c->w(), c->h());
	}
	ycur += c->h();
	if(ycur >= ypos_ + h()) done = true;
      }
    }
  }
  fl_pop_clip();
  draw_child(scrollbar);
}

int Fl_ScrollPack::handle(int ev)
{
  bool in_sbar = Fl::event_inside(&scrollbar);
  if(in_sbar && (ev == FL_ENTER || ev == FL_MOVE)) {
    if(Fl::belowmouse() != &scrollbar) Fl::belowmouse(&scrollbar);
    return scrollbar.handle(ev);
  }
  if(in_sbar || (Fl::belowmouse() == &scrollbar && ev == FL_DRAG)) {
    int r = scrollbar.handle(ev);
    if(r) return r;
  }
  return Fl_Group::handle(ev);
}
