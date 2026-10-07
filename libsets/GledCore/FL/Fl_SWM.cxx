// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include <FL/Fl_SWM.H>

#include <FL/Fl.H>
#include <FL/Fl_Browser_.H>
#include <FL/Fl_Input_.H>
#include <FL/Fl_Menu_.H>
#include <FL/Fl_Value_Output.H>
#include <FL/Fl_Scroll.H>
#include <FL/fl_draw.H>

// #include <stdio.h>

/**************************************************************************/
// Fl_SWM_Client
/**************************************************************************/

Fl_SWM_Client::~Fl_SWM_Client()
{
  if(swm_manager && swm_window) {
    swm_manager->remove_window(swm_window);
  }
  delete swm_size_range;
}

void Fl_SWM_Client::set_size_range(SWM_Size_Range* sr)
{
  delete swm_size_range;
  swm_size_range = sr;
}

void Fl_SWM_Client::trace_swm_manager(Fl_Widget* w)
{
  // Searches for SWManager in w's ancestors ... then forces it to adopt itself

  if(swm_manager) return;
  Fl_SWM_Manager* mgr = Fl_SWM_Manager::search_manager(w);
  if(mgr) mgr->adopt_window(dynamic_cast<Fl_Window*>(this));
}

void Fl_SWM_Client::show_swm_ctrl()
{ if(swm_manager) swm_manager->show_ctrl(); }

void Fl_SWM_Client::hotspot_swm_ctrl()
{ if(swm_manager) swm_manager->hotspot_ctrl(); }

/**************************************************************************/
// Fl_SWM_Manager
/**************************************************************************/

int
Fl_SWM_Manager::calc_width(Fl_Group* g, int w, int fs) {
  if(w == 0) {
    fl_font(g->labelfont(), fs);
    return (int) fl_width('X') + 2;
  } else {
    return w;
  }
}

namespace{
  int foosoo(int nfs, int ofs, int cs) {
    // Attempting simple font scaling
    // Expects groups to be set to default sizes
    if(cs == ofs) return nfs;
    return nfs - (ofs - cs);
  }
}

void
Fl_SWM_Manager::resize_group(Fl_Group* g, int ow, int oh,
				       int nw, int nh, int nfs)
{
  // Resizes group from old->new settings.
  // Takes care of certain peculiarities of most (?) Fltk widgets.

  Fl_SWM_Control* swmctrl = dynamic_cast<Fl_SWM_Control*>(g);

  int ofs = g->labelsize();
  // Fl_Window* gwin = dynamic_cast<Fl_Window*>(g);
  if(g->type() >= FL_WINDOW) {
    Fl_Widget* resize_store = g->resizable();
    g->resizable(0);
    if(g->parent() == 0) {
      g->resize(g->x(), g->y(), g->w()/ow*nw, g->h()/oh*nh);
    } else {
      g->resize(g->x()/ow*nw, g->y()/oh*nh, g->w()/ow*nw, g->h()/oh*nh);
    }
    g->resizable(resize_store);
  } else {
    g->Fl_Widget::resize(g->x()/ow*nw, g->y()/oh*nh, g->w()/ow*nw, g->h()/oh*nh);
  }
  g->labelsize(nfs);

  if(swmctrl == 0 || (swmctrl && swmctrl->swm_ctrl_keep_child_sizes == false)) {
    for(int i=0; i<g->children(); ++i) {
      Fl_Widget* w = g->child(i);
      Fl_Group* wg = dynamic_cast<Fl_Group*>(w);
      if(wg) {
	resize_group(wg, ow, oh, nw, nh, nfs);
      } else {
	w->resize(w->x()/ow*nw, w->y()/oh*nh, w->w()/ow*nw, w->h()/oh*nh);
	w->labelsize( foosoo(nfs, ofs, w->labelsize()) );
	if(Fl_Browser_* cw = dynamic_cast<Fl_Browser_*>(w))
	  cw->textsize( foosoo(nfs, ofs, cw->textsize()) );
	else if(Fl_Input_* cw = dynamic_cast<Fl_Input_*>(w))
	  cw->textsize( foosoo(nfs, ofs, cw->textsize()) );
	else if(Fl_Menu_* cw = dynamic_cast<Fl_Menu_*>(w))
	  cw->textsize( foosoo(nfs, ofs, cw->textsize()) );
	else if(Fl_Value_Input* cw = dynamic_cast<Fl_Value_Input*>(w))
	  cw->textsize( foosoo(nfs, ofs, cw->textsize()) );
	else if(Fl_Value_Output* cw = dynamic_cast<Fl_Value_Output*>(w))
	  cw->textsize( foosoo(nfs, ofs, cw->textsize()) );
      }
    }
    g->init_sizes();
  }

  g->redraw();
}

/**************************************************************************/

void Fl_SWM_Manager::_init(int fs, int h, int w)
{
  // Instantiates control window
  // Initializes size variables.
  // w = 0 means width of 'X'+2
  {
    Fl_Group* exc = Fl_Group::current();
    Fl_Group::current(0);

    wCtrl = new Fl_Window(12, 4, "Size Control");

    wWSkip = new Fl_Value_Input(8,0,4,1,"W Skip:");
    wWSkip->range(0, 250); wWSkip->step(1,1);

    wHSkip = new Fl_Value_Input(8,1,4,1,"H Skip:");
    wHSkip->range(0, 250); wHSkip->step(1,1);

    wFontSize = new Fl_Value_Input(8,2,4,1,"Font size:");
    wFontSize->range(4, 250); wFontSize->step(1,1);

    Fl_Button* b;
    b = new Fl_Button(0,3,4,1,"Try");
    b->callback((Fl_Callback*)Fl_SWM_Manager::s_try, this);
    b = new Fl_Button(4,3,4,1,"XX");
    b->callback((Fl_Callback*)Fl_SWM_Manager::s_restore, this);
    b = new Fl_Button(8,3,4,1,"Go");
    b->callback((Fl_Callback*)Fl_SWM_Manager::s_go, this);

    wCtrl->end();
    Fl_Group::current(exc);
  }

  ctr_width  = ctr_height = 1; ctr_fs = 0;
  bu_w  = w;
  bu_h  = h;
  old_fs     = bu_fs = fs;
  old_width  = bu_width = calc_width(wCtrl, bu_w, bu_fs);
  old_height = bu_height= bu_h + bu_fs;

  restore_sizes();
  in_rescale = false;
}

Fl_SWM_Manager::Fl_SWM_Manager(int fs, int h, int w)
{
  _init(fs, h, w);
}

Fl_SWM_Manager::Fl_SWM_Manager(const Fl_SWM_Manager* copy)
{
  if(copy)
    _init(copy->cell_fontsize(), copy->setting_h(), copy->setting_w());
  else
    _init();
}

Fl_SWM_Manager::~Fl_SWM_Manager() {
  // Unregisters all clients. The windows are not deleted.
  for(lpFl_Window_i i = mWindows.begin(); i!=mWindows.end(); ++i) {
    if(Fl_SWM_Client* c = dynamic_cast<Fl_SWM_Client*>(*i)) {
      c->swm_manager = 0;
    }
  }
  delete wCtrl;
}

/**************************************************************************/

void Fl_SWM_Manager::adopt_window(Fl_Window* w)
{
  in_rescale = true;
  resize_window(w, 1, 1, old_width, old_height, old_fs);
  in_rescale = false;

  mWindows.push_back(w);
  if(Fl_SWM_Client* c = dynamic_cast<Fl_SWM_Client*>(w)) {
    c->swm_manager = this; c->swm_window = w;
    // printf("Adopting %p\n", w);
  }
}

void Fl_SWM_Manager::prepare_group(Fl_Group* g)
{
  // Resizes group g from canonical size to current SWM sizes
  // Use cases:
  // a) addition into SWM client (eg insertion of groups into a Fl_Pack)
  // b) resize from scratch

  resize_group(g, 1, 1, old_width, old_height, old_fs);
}

void Fl_SWM_Manager::resize_window(Fl_Window* w, int ow, int oh,
					      int nw, int nh, int nfs)
{
  // Resizes window from old character width/height to the new one
  //w->size_range(0,0,4096,4096);
  resize_group(w, ow, oh, nw, nh, nfs);
  Fl_SWM_Client* c = dynamic_cast<Fl_SWM_Client*>(w);
  if(c && c->swm_size_range) {
    SWM_Size_Range& s = *c->swm_size_range;
    w->size_range(nw*s.wl, nh*s.hl, nw*s.wh, nh*s.hh, nw*s.dwf, nh*s.dhf);
  }
}

void Fl_SWM_Manager::add_window(Fl_Window* w)
{
  // printf("Fl_SWM_Manager::add_window %p\n", w);
  mWindows.push_back(w);
}

void Fl_SWM_Manager::remove_window(Fl_Window* w)
{
  // printf("Fl_SWM_Manager::remove_window %p\n", w);
  mWindows.remove(w);
}

/**************************************************************************/

void Fl_SWM_Manager::kill_all_windows()
{
  while(!mWindows.empty()) {
    Fl_Window* w = mWindows.back();
    mWindows.pop_back();
    delete w;
  }
  mWindows.clear();
  wCtrl->hide();
}

/**************************************************************************/

void Fl_SWM_Manager::try_sizes()
{
  // Resizes control window to the values set in widgets.
  // Meant as a test of the settings.

  bu_width = ctr_width; bu_height = ctr_height; bu_fs = ctr_fs;
  bu_w = ctr_w; bu_h = ctr_h;
  ctr_w     = (int) wWSkip->value();
  ctr_h	    = (int) wHSkip->value();
  ctr_fs    = (int) wFontSize->value();
  ctr_width = calc_width(wCtrl, ctr_w, ctr_fs);
  ctr_height = ctr_fs + ctr_h;
  resize_window(wCtrl, bu_width, bu_height, ctr_width, ctr_height, ctr_fs);
  wCtrl->redraw();
}

void Fl_SWM_Manager::restore_sizes()
{
  // Restores sizes of the control window.

  wWSkip->value(bu_w); wHSkip->value(bu_h); wFontSize->value(bu_fs);

  resize_group(wCtrl, ctr_width, ctr_height, bu_width, bu_height, bu_fs);
  wCtrl->redraw();

  ctr_width = bu_width; ctr_height = bu_height; ctr_fs = bu_fs;
  ctr_w = bu_w; ctr_h = bu_h;
}

void
Fl_SWM_Manager::go_sizes() {
  // Applies current settings to all clients.
  try_sizes();

  in_rescale = true;
  for(lpFl_Window_i i = mWindows.begin(); i!=mWindows.end(); ++i) {
    resize_window(*i, old_width, old_height, ctr_width, ctr_height, ctr_fs);
    (*i)->redraw();
  }
  in_rescale = false;

  old_width  = ctr_width;
  old_height = ctr_height;
  old_fs     = ctr_fs;
}

/**************************************************************************/

Fl_SWM_Manager*
Fl_SWM_Manager::search_manager(Fl_Widget* w) {
  // Static. Traverses w's ancestors and returns the first Manager
  // found (or 0).
  while(w) {
    Fl_SWM_Client* c = dynamic_cast<Fl_SWM_Client*>(w);
    if(c) return c->get_swm_manager();
    w = w->parent();
  }
  return 0;
}
