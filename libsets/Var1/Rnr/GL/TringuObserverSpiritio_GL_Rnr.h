// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef Var1_TringuObserverSpiritio_GL_RNR_H
#define Var1_TringuObserverSpiritio_GL_RNR_H

#include <Glasses/TringuObserverSpiritio.h>
#include <Rnr/GL/Spiritio_GL_Rnr.h>

namespace gled {

class TringuObserverSpiritio_GL_Rnr : public Spiritio_GL_Rnr
{
private:
  void _init();

protected:
  TringuObserverSpiritio*        mTringuObserverSpiritio;

public:
  TringuObserverSpiritio_GL_Rnr(TringuObserverSpiritio* idol);
  virtual ~TringuObserverSpiritio_GL_Rnr();

  // virtual void Draw(RnrDriver* rd);

  virtual int  Handle(RnrDriver* rd, Fl_Event& ev);

}; // endclass TringuObserverSpiritio_GL_Rnr

} // endnamespace gled

#endif
