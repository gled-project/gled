// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef Audio1_AlBuffer_H
#define Audio1_AlBuffer_H

#include <Glasses/ZGlass.h>

namespace gled {

class AlBuffer : public ZGlass
{
  MAC_RNR_FRIENDS(AlBuffer);

private:
  void _init();

protected:
  TString     mFile;      //  X{GS} 7 Filor()

  Int_t       mUseCount;  //! X{G} Not used, I fear.
  UInt_t      mAlBuf;	  //! X{G}

  Int_t       mFrequency; //! X{G} 7 ValOut()
  Int_t       mSize;      //! X{G} 7 ValOut()
  Int_t       mBits;      //! X{G} 7 ValOut(-width=>4, -join=>1)
  Int_t       mChannels;  //! X{G} 7 ValOut(-width=>4)
  Float_t     mDuration;  //! X{G} 7 ValOut()

  void clear_buffer_props();

public:
  AlBuffer(const Text_t* n="AlBuffer", const Text_t* t=0);
  virtual ~AlBuffer();

  void Load(); // X{E} 7 MButt()

#include "AlBuffer.h7"
  ClassDef(AlBuffer, 1);
}; // endclass AlBuffer

} // endnamespace gled

#endif
