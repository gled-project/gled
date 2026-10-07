// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef Geom1_MultiBoard_H
#define Geom1_MultiBoard_H

#include <Glasses/Board.h>

namespace gled {

class ZHashList;

class MultiBoard : public Board
{
  MAC_RNR_FRIENDS(MultiBoard);

private:
  void _init();

protected:
  ZLink<ZHashList>	mSlides;	// X{gS} L{}

public:
  MultiBoard(const Text_t* n="MultiBoard", const Text_t* t=0) :
    Board(n,t) { _init(); }

  virtual void AdEnlightenment();

  void First(); // X{E} 7 MButt(-join=>1)
  void Last();  // X{E} 7 MButt()
  void Prev();  // X{E} 7 MButt(-join=>1)
  void Next();  // X{E} 7 MButt()

#include "MultiBoard.h7"
  ClassDef(MultiBoard, 1); // Board with a sequence of images/slides.
}; // endclass MultiBoard


} // endnamespace gled

#endif
