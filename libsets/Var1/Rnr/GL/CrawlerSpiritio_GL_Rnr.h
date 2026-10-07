// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef Var1_CrawlerSpiritio_GL_RNR_H
#define Var1_CrawlerSpiritio_GL_RNR_H

#include <Glasses/CrawlerSpiritio.h>
#include <Rnr/GL/Spiritio_GL_Rnr.h>

namespace gled {

class CrawlerSpiritio_GL_Rnr : public Spiritio_GL_Rnr
{
private:
  void _init();

protected:
  CrawlerSpiritio*        mCrawlerSpiritio;

public:
  CrawlerSpiritio_GL_Rnr(CrawlerSpiritio* idol);
  virtual ~CrawlerSpiritio_GL_Rnr();

  virtual void Draw(RnrDriver* rd);

  virtual int HandleMouse(RnrDriver* rd, Fl_Event& ev);

}; // endclass CrawlerSpiritio_GL_Rnr

} // endnamespace gled

#endif
