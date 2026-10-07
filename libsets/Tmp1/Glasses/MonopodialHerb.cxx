// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "MonopodialHerb.h"
using namespace gled;
#include "MonopodialHerb.c7"

// MonopodialHerb

//______________________________________________________________________________
//
//

//==============================================================================

void MonopodialHerb::_init()
{
  mStart.x('I', 9).x('a', 13);
}

MonopodialHerb::MonopodialHerb(const Text_t* n, const Text_t* t) :
  GrowingPlant(n, t) 
{
  _init();
}

MonopodialHerb::~MonopodialHerb()
{}

//==============================================================================
void MonopodialHerb::SegmentStepTime(Segments_i oldRef, Segments_t& in, Segments_t& out)
{
  out.s('/', 137);
  out.s('+', 17);
  out.s('-', 17);
  
  Segment& seg = *oldRef;
  switch (seg.mType) {
    case 'a': 
      out.s('I', 10).s('&', 70);
      out.x("[&L]/I");
      if (seg.mParam1 > 0)
        out.x('a', seg.mParam1-1); 
      else 
        out.x('A', -1);   
      break;
      
    case  'A':
      out.s('&', 18).s('u', 4).s('X', 5);
      out.x("[&uFF").x('I', 10).x('I', 5).x("XKKKK]/IA");
      break;
      
    case 'I':
      out.x('F', 1); 
      if (seg.mParam1 > 0) out.x('I', seg.mParam1-1);
      break;
      
    case 'u':
      out.x('&', 9); 
      if (seg.mParam1 > 0) out.x('u', seg.mParam1-1);
      break;
    
    case 'L':
      out.s('I', 7);
      out.x("[{.-FI+FI+FI}] [{.+FI-FI-FI}]");
      break;

    case 'K':
      out.s('I', 2).s('/', 90);
      out.x("\['&{.+FI--FI}] ['&{.-FI++FI}]/");
      break; 
      
    case 'X':
      if (seg.mParam1 > 0)
        out.x('X', seg.mParam1 -1);
      else
        out.s('^', 50).x("^[[-GGGG++[GGG[++G{.].].]. ++GGGG.--GGG.--G.}]%");
      break;    
      
    default:
      // copy existing
      out.x(seg.mType, seg.mParam1);
      break;
  }
}
  