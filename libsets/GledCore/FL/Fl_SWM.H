// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef Fl_SWM_H
#define Fl_SWM_H

#include <FL/Fl_Window.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Value_Input.H>

#include <list>

typedef std::list<Fl_Window*>		lpFl_Window_t;
typedef std::list<Fl_Window*>::iterator	lpFl_Window_i;

class Fl_SWM_Manager;

struct SWM_Size_Range
{
  int wl, hl, wh, hh; // low/high width/height
  int dwf, dhf;	      // delta w/h factor
  SWM_Size_Range(int a, int b, int c, int d, int e=1, int f=1) :
    wl(a), hl(b), wh(c), hh(d), dwf(e), dhf(f) {}
};

class Fl_SWM_Control
{
  friend class Fl_SWM_Manager;
protected:
  bool		swm_ctrl_keep_child_sizes;
  
public:
  Fl_SWM_Control() : swm_ctrl_keep_child_sizes(false) {}
};

/**************************************************************************/
// Fl_SWM_Client
/**************************************************************************/

class Fl_SWM_Client
{
  // Interface to provide a Fl_Window derived class with Fl_SWM info
protected:
  Fl_SWM_Manager* swm_manager;
  Fl_Window*	  swm_window;
  SWM_Size_Range* swm_size_range;

  static void s_show_swm_ctrl(Fl_Button* b, Fl_SWM_Client* c)
  { c->show_swm_ctrl(); }
  static void s_hotspot_swm_ctrl(Fl_Button* b, Fl_SWM_Client* c)
  { c->hotspot_swm_ctrl(); }

public:
  Fl_SWM_Client(Fl_SWM_Manager* mgr=0) : 
    swm_manager(mgr), swm_window(0), swm_size_range(0) {}
  virtual ~Fl_SWM_Client();

  void set_size_range(SWM_Size_Range* sr);

  virtual Fl_SWM_Manager* get_swm_manager() const { return swm_manager; }
  virtual void trace_swm_manager(Fl_Widget* w);

  void show_swm_ctrl();
  void hotspot_swm_ctrl();

  void set_swm_show_cb(Fl_Button* b)
  { b->callback((Fl_Callback*) s_show_swm_ctrl, this); }
  void set_swm_hotspot_cb(Fl_Button* b)
  { b->callback((Fl_Callback*) s_hotspot_swm_ctrl, this); }

  friend class Fl_SWM_Manager;
};

/**************************************************************************/
// Fl_SWM_Manager
/**************************************************************************/

class Fl_SWM_Manager
{
  // Stores size information and maintains own window for size control
  // and a list (map?) of windows under its control

  // Size state

  int			old_width, old_height, old_fs;
  int			ctr_width, ctr_height, ctr_fs, ctr_w, ctr_h;
  int			bu_width, bu_height, bu_fs, bu_w, bu_h;

  bool			in_rescale;

  void _init(int fs=14, int h=6, int w=0);

  static int calc_width(Fl_Group* g, int w, int fs);
protected:
  // Size control window and its widgets

  Fl_Window*		wCtrl;
  Fl_Value_Input*	wWSkip; // if non 0 hardcodes the width (else width 'X')
  Fl_Value_Input*	wHSkip; // added to fontsize
  Fl_Value_Input*	wFontSize;

  // Adopted windows

  lpFl_Window_t		mWindows;

public:
  static void resize_group(Fl_Group* g, int ow, int oh, int nw, int nh, int nfs);

  // Methods

  Fl_SWM_Manager(int fs=14, int h=6, int w=0);
  Fl_SWM_Manager(const Fl_SWM_Manager* copy);
  virtual ~Fl_SWM_Manager();

  void adopt_window(Fl_Window* w);
  void prepare_group(Fl_Group* g);
  void resize_window(Fl_Window* w, int ow, int oh, int nw, int nh, int nfs);

  void add_window(Fl_Window* w);
  void remove_window(Fl_Window* w);

  void kill_all_windows();

  int  cell_w() const { return in_rescale ? ctr_width : old_width; }
  int  cell_h() const { return in_rescale ? ctr_height :old_height; }
  int  cell_fontsize() const { return in_rescale ? ctr_fs : old_fs; }
  int  setting_h() const { return int(wHSkip->value()); }
  int  setting_w() const { return int(wWSkip->value()); }
  bool in_rescale_p() const { return in_rescale; }

  void try_sizes();
  void restore_sizes();
  void go_sizes();
  static void s_try(Fl_Widget* w, Fl_SWM_Manager* x)
  { x->try_sizes(); }
  static void s_restore(Fl_Widget* w, Fl_SWM_Manager* x)
  { x->restore_sizes(); }
  static void s_go(Fl_Widget* w, Fl_SWM_Manager* x)
  { x->go_sizes(); }

  void show_ctrl()    { wCtrl->show(); }
  void hotspot_ctrl() { wCtrl->hotspot(wCtrl->w()/2, wCtrl->h()/2); wCtrl->show(); }

  static Fl_SWM_Manager* search_manager(Fl_Widget* w);
};

#endif
