// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef GledCore_Text_GL_RNR_H
#define GledCore_Text_GL_RNR_H

#include <Glasses/Text.h>
#include <Rnr/GL/ZNode_GL_Rnr.h>

namespace gled {

class Text_GL_Rnr : public ZNode_GL_Rnr {
private:
  void _init();

protected:
  Text*	      mText;
  RnrModStore mFontRMS;

public:
  Text_GL_Rnr(Text* idol) : ZNode_GL_Rnr(idol),
			    mText(idol), mFontRMS(ZRlFont::FID())
  { _init(); }

  virtual void Draw(RnrDriver* rd);
  virtual void Render(RnrDriver* rd);

}; // endclass Text_GL_Rnr

} // endnamespace gled

#endif
