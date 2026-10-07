// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include <FL/Fl_OutputPack.H>
#include <FL/Fl_Output.H>
#include <FL/Fl.H>

#include <FL/fl_draw.H>

#include <string.h>

Fl_OutputPack::Fl_OutputPack(int x, int y, int w, int h, const char* t) :
  Fl_Scroll(x, y, w, h, t),
  _base_size_mod(0), _base_skip_mod(0), _max_lines(100), _keep_pos(false),
  _max_w(0), _sum_h(0),
  swm_mgr(0)
{
  box(FL_FLAT_BOX);

  pack = new Pack(x, y, 1, 1);
  pack->type(FL_VERTICAL);
  pack->box(FL_NO_BOX);
  pack->end();
  end();
  resizable(this);

  scrollbar.resize(x+w-2, y, 2, h);
  hscrollbar.resize(0,0, w-2, 1);
}

void Fl_OutputPack::add_line(const char* text, Fl_Color col, int size_mod)
{
  if(swm_mgr == 0) swm_mgr = Fl_SWM_Manager::search_manager(this);
  int mw = 0, mh = 0;
  fl_font(pack->labelfont(), swm_mgr->cell_fontsize() + _base_size_mod + size_mod);
  fl_measure(text, mw, mh, 0);
  int ow = mw + (swm_mgr->cell_w() - (mw % swm_mgr->cell_w()));
  int oh = swm_mgr->cell_h() + _base_size_mod + size_mod + _base_skip_mod;
  Fl_Output* o = new Fl_Output(0,0,ow,oh);
  o->value(text);
  o->box(FL_FLAT_BOX);
  o->color(pack->color());
  o->textcolor(col ? col : pack->labelcolor());
  o->textsize(swm_mgr->cell_fontsize() + _base_size_mod + size_mod);
  pack->add(o);
  if(ow > _max_w) {
    _max_w = ow;
    pack->size(_max_w, pack->h());
  }
  _sum_h += oh;
  _remove_lines();

  if(!_keep_pos && _sum_h >= h() + yposition())
    scroll_to(0, _sum_h - h());

  redraw();
}

void Fl_OutputPack::max_lines(int ml)
{
  _max_lines = ml;
  _remove_lines();
}

void Fl_OutputPack::bg_color(Fl_Color c)
{
  color(c);
  pack->color(c);
  Fl_Widget* const* a = pack->array();
  if(a) for(int i=pack->children(); i--; ++a) (*a)->color(c);
  redraw();
}

/**************************************************************************/

int Fl_OutputPack::handle(int ev)
{
  if(ev == FL_PUSH) Fl::focus(this);
  return Fl_Scroll::handle(ev);
}

/**************************************************************************/

void Fl_OutputPack::_remove_lines()
{
  if(pack->children() > _max_lines) {
    while(pack->children() > _max_lines) {
      Fl_Widget* w = pack->child(0);
      pack->remove(*w);
      _sum_h -= w->h();
      delete w;
    }
    int maxw=0;
    Fl_Widget* const* a = pack->array();
    if(a) {
      for(int i=pack->children(); i--; ++a)
	if((*a)->w() > maxw) maxw = (*a)->w();
    }
    if(maxw != _max_w) {
      _max_w = maxw;
      pack->size(_max_w, pack->w());
    }
    redraw();
  }
}
