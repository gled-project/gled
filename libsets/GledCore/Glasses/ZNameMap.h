// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef GledCore_ZNameMap_H
#define GledCore_ZNameMap_H

#include <Glasses/ZList.h>

namespace gled {

class ZNameMap : public ZList,
                 public ZGlass::NameChangeCB
{
  MAC_RNR_FRIENDS(ZNameMap);

private:
  void _init();

protected:
  // AList ... ZList
  virtual void new_element_check(ZGlass* lens);
  virtual void clear_list();

  // ZList
  virtual void on_insert(ZList::iterator it);
  virtual void on_remove(ZList::iterator it);
  virtual void on_rebuild();

  virtual void insert_with_placement(ZGlass* lens);

  //----------------------------------------------------------------------

  Bool_t      bKeepSorted;      // X{GE} 7 Bool(-join=>1)
  Bool_t      bWarnEqualName;   // X{GS} 7 Bool()

  typedef std::multimap<TString, ZList::iterator>           mName2Iter_t;
  typedef std::multimap<TString, ZList::iterator>::iterator mName2Iter_i;
  typedef std::pair<TString, ZList::iterator>               mName2Iter_pair;
  typedef std::pair<mName2Iter_i, mName2Iter_i>             mName2Iter_i_pair;

  mName2Iter_t mItMap; //!

  void shoot_sort_mir();

public:
  ZNameMap(const Text_t* n="ZNameMap", const Text_t* t=0);
  virtual ~ZNameMap();

  void SetKeepSorted(Bool_t keep_sorted);

  virtual ZGlass* GetElementByName (const TString& name);
  virtual Int_t   GetElementsByName(const TString& name, lpZGlass_t& dest);

  virtual void SortByName();

  void DumpNameMap(); //! X{E} 7 MButt()

  //----------------------------------------------------------------------
  // Functions that add elements - bKeepSorted overrides placement

  virtual void PushBack(ZGlass* lens);                    // Exported in ZList
  virtual void PushFront(ZGlass* lens);                   // id.
  virtual void InsertById(ZGlass* lens, Int_t before_id); // id.

  //----------------------------------------------------------------------
  // ZGlass::NameChangeCB
  virtual void name_change_cb(ZGlass* lens, const TString& new_name);

#include "ZNameMap.h7"
  ClassDef(ZNameMap, 1);
}; // endclass ZNameMap


} // endnamespace gled

#endif
