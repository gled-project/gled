// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef FL_OUTPUTPACK_H
#define FL_OUTPUTPACK_H

#include <FL/Fl_Pack.H>
#include <FL/Fl_Scroll.H>
#include <FL/Fl_SWM.H>

class Fl_OutputPack : public Fl_Scroll
{
private:
  void _remove_lines();

protected:
  int	_base_size_mod;
  int	_base_skip_mod;
  int	_max_lines;
  bool	_keep_pos;

  int	_max_w;
  int	_sum_h;

  Fl_SWM_Manager* swm_mgr;

  class Pack : public Fl_Pack, public Fl_SWM_Control
  {
  public:
    Pack(int x, int y, int w, int h, const char* t=0) :
      Fl_Pack(x,y,w,h,t), Fl_SWM_Control()
    { swm_ctrl_keep_child_sizes = true; }
  };

public:
  Fl_Pack*	pack;

  Fl_OutputPack(int x, int y, int w, int h, const char* t=0);

  void add_line(const char* text, Fl_Color col=(Fl_Color)0, int size_mod=0);

  int  base_size_mod() const { return _base_size_mod; }
  void base_size_mod(int bsm) { _base_size_mod = bsm; }
  int  base_skip_mod() const { return _base_skip_mod; }
  void base_skip_mod(int bsm) { _base_skip_mod = bsm; }
  int  max_lines() const { return _max_lines; }
  void max_lines(int ml);
  bool keep_pos() const { return _keep_pos; }
  void keep_pos(bool k) { _keep_pos = k; }

  void bg_color(Fl_Color c);
  void fg_color(Fl_Color c) { pack->labelcolor(c); }

  virtual int handle(int ev);
};

#endif
