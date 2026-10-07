// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef GledCore_MTW_Layout_H
#define GledCore_MTW_Layout_H

#include <Gled/GledTypes.h>
#include <GledView/GledViewNS.h>
#include <FL/Fl_SWM.H>
#include <FL/Fl_Window.H>

namespace gled {

class FTW_Nest;

class MTW_Layout : public Fl_Window, public Fl_SWM_Client {

public:
  struct Member {
    GledViewNS::WeedInfo*	fWeedInfo;
    int				fW;
    Member(GledViewNS::WeedInfo* wi, int w=0) : fWeedInfo(wi), fW(w) {}
  };
  typedef std::list<Member>			lMember_t;
  typedef std::list<Member>::iterator	lMember_i;

  struct Class {
    GledNS::ClassInfo*		fClassInfo;
    lMember_t			fMembers;
    int				fFullW;
    Class(GledNS::ClassInfo* ci) : fClassInfo(ci), fFullW(0) {}
  };
  typedef std::list<Class>			lClass_t;
  typedef std::list<Class>::iterator		lClass_i;

protected:
  FTW_Nest*	mNest;		// X{g}

  lClass_t	mClasses;	// X{r}
  bool		bIsValid;	// X{g}

  Fl_Input*	wLaySpecs;	// X{g}

public:
  MTW_Layout(FTW_Nest* nest);
  void Parse(int cell_w=0);
  int  CountSubViews(ZGlass* lens);

  Fl_Group* CreateLabelGroup();

#include "MTW_Layout.h7"
}; // endclass MTW_Layout

} // endnamespace gled

#endif
