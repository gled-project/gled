// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef GledCore_GLTextNS_H
#define GledCore_GLTextNS_H

#include <Gled/GledTypes.h>
#include <GL/glew.h>

class FTFont;

namespace gled {

class RnrDriver;
class ZColor;

namespace GLTextNS
{
  struct BoxSpecs
  {
    int     lm, rm, tm, bm;
    int     lineskip;
    char    align;
    TString pos;

    void _init() { align = 'l'; lineskip = 0; }

    BoxSpecs()
    { lm = rm = 3; tm = 0; bm = 2; _init(); }

    BoxSpecs(int lr, int tb)
    { lm = rm = lr; tm = bm = tb; _init(); }

    BoxSpecs(int l, int r, int t, int b)
    { lm = l; rm = r; tm = t; bm = b; _init(); }
  };

  struct TextLineData
  {
    float   width, ascent, descent, hfull;
    TString text;

    TextLineData(FTFont *ftf, const TString& line);
  };

  Float_t MeasureWidth(FTFont *ftf, const TString& txt);
  Float_t MeasureWidth(FTFont *ftf, const TString& txt,
			      Float_t& ascent, Float_t& descent);

  void RnrTextBar(RnrDriver* rd, const TString& text);

  void RnrTextBar(RnrDriver* rd, const TString& text,
			 BoxSpecs& bs, float zoffset=0);

  void RnrTextPoly(RnrDriver* rd, const TString& text);

  void RnrText(RnrDriver* rd, const TString& text,
		      int x, int y, float z,
		      const ZColor* front_col, const ZColor* back_col=0);

  void RnrTextAt(RnrDriver* rd, const TString& text,
			int x, int yrow, float z,
			const ZColor* front_col, const ZColor* back_col=0);
}

} // endnamespace gled

#endif
