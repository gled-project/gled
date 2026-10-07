// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef Audio1_AlContext_H
#define Audio1_AlContext_H

#include <Glasses/ZNode.h>

#ifndef __ROOTCLING__
#include <AL/alut.h>
#else
class ALCdevice;
class ALCcontext;
#endif

namespace gled {

class AlContext : public ZNode
{
  MAC_RNR_FRIENDS(AlContext);

private:
  void _init();

protected:
  ALCdevice*  mDevice;  //! X{g}
  ALCcontext* mContext; //! X{g}

public:
  AlContext(const Text_t* n="AlContext", const Text_t* t=0) :
    ZNode(n,t) { _init(); }

  void Open();  // X{E}  7 MButt(-join=>1)
  void Close(); // X{E}  7 MButt()

#include "AlContext.h7"
  ClassDef(AlContext, 1);
}; // endclass AlContext


} // endnamespace gled

#endif
