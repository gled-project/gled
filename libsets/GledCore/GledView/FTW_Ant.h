// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef GledCore_FTW_Ant
#define GledCore_FTW_Ant

#include <Eye/OptoStructs.h>
#include "FTW.h"

#include <FL/Fl_Group.H>

class Fl_Button;

namespace gled {

class FTW_Leaf;

class FTW_Ant : public Fl_Group, public OptoStructs::ZLinkView {
  friend class FTW_Leaf;
  friend class FTW_Nest;
  friend class FTW_Shell;
  friend class FTW::Locator;
protected:
  FTW_Leaf*		mParent; // X{g} needed for method request propagation, decollapses
  FTW_Leaf*		mLeaf;	 // X{g} sub-leaf (if expanded)
  bool			bExpanded;
  bool			bExpandIfCan;
  // Sub weeds
  Fl_Button*		wExpander;
  Fl_Button*		wListExpander;
  FTW::NameButton*	wName;

  void resize_weeds(int name_w, int butt_w, int h);
  void label_weeds();
  void recolor_name();
  void modify_box_color(Fl_Color mod, bool on_p);

public:
  FTW_Ant(OptoStructs::ZLinkDatum* ld, FTW_Leaf* p);
  virtual ~FTW_Ant();

  virtual void LinkViewUpdate();

  void CollExp();
  void ListExpander(); // link + list members

  bool IsSet();
  bool IsList();

#include "FTW_Ant.h7"
}; // endclass FTW_Ant

typedef std::list<FTW_Ant*>			lpFTW_Ant_t;
typedef std::list<FTW_Ant*>::iterator	lpFTW_Ant_i;

} // endnamespace gled

#endif
