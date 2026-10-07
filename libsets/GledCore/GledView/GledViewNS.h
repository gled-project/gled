// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef GledCore_GledViewNS_H
#define GledCore_GledViewNS_H

#include <Gled/GledNS.h>

class TMessage;
class Fl_Widget;

namespace gled {

class ZGlass;
class ZMIR;
class Eye;
class MTW_View;
class MTW_SubView;
class FTW_Ant;

namespace GledViewNS
{
  // Low level: weed info

  typedef Fl_Widget*	(*WeedCreator_foo)    (MTW_SubView*);
  typedef void		(*WeedCallback_foo)   (Fl_Widget*, MTW_SubView*);
  typedef void		(*WeedUpdate_foo)     (Fl_Widget*, MTW_SubView*);
  typedef void		(*CtxCallCreator_foo) (TBuffer*);

  class MethodInfo {};		// Null so far

  class DataMemberInfo {};	// Null so far

  class LinkMemberInfo {};

  class WeedInfo : public GledNS::InfoBase
  {
  public:
    bool		bIsLinkWeed;
    Int_t		fWidth;
    Int_t		fHeight;
    bool		bLabel;
    bool		bLabelInside;
    bool		bCanResize;
    bool		bJoinNext;
    WeedCreator_foo	fooWCreator;
    WeedCallback_foo	fooWCallback;
    WeedUpdate_foo	fooWUpdate;

    WeedInfo(const TString& s) : InfoBase(s) {}
  };

  typedef std::list<WeedInfo*>		lpWeedInfo_t;
  typedef std::list<WeedInfo*>::iterator	lpWeedInfo_i;

  // Intermediate level: ClassInfo, RnrCreator and SubView info

  typedef MTW_SubView*	(*SubViewCreator_foo)(GledNS::ClassInfo* ci, MTW_View* v, ZGlass* g);

  class ClassInfo
  {
  public:
    SubViewCreator_foo		fooSVCreator;
    lpWeedInfo_t		fWeedList;

    //----------------------------------------------------------------

    WeedInfo*		FindWeedInfo(const TString& name, bool recurse=false,
				     GledNS::ClassInfo* true_class=0);
  };

  // High level: per LibSet information

  class LibSetInfo {};	// Now empty ... rnr support in base.

  /**************************************************************************/
  // Variables
  /**************************************************************************/


  /**************************************************************************/
  // Functions
  /**************************************************************************/

  Int_t LoadSoSet(const TString& lib_set);
  Int_t InitSoSet(const TString& lib_set);
  void	BootstrapViewSet(LID_t lid, const TString& libset);
  void  BootstrapClassInfo(ClassInfo* c_info);

  TString FabricateViewLibName(const TString& libset);
  TString FabricateViewInitFoo(const TString& libset);
  TString FabricateViewUserInitFoo(const TString& libset);

  // GUI magick

  extern int no_symbol_label;
  extern int menubar_box;

} // GledViewNS

} // endnamespace gled

#endif
