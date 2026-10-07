// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

//__________________________________________________________________________
// ScreenDumper
//
//

#include "ScreenDumper.h"

using namespace gled;

#include "ScreenDumper.c7"

/**************************************************************************/

void ScreenDumper::_init()
{
  bWaitSignal   = false;
  bDumpImage    = false;
  mFileNameFmt  = "screendumper/img%04d";
  mNTiles       = 1;
  bCopyToScreen = false;

  mDumpID = 0;
}

/**************************************************************************/

void ScreenDumper::DumpScreen()
{
  if(mPupil != 0)
  {
    if (bDumpImage)
    {
      TString fname(GForm(mFileNameFmt.Data(), mDumpID++));
      if (bWaitSignal)
        mPupil->DumpImageWaitSignal(fname, mNTiles, bCopyToScreen);
      else
        mPupil->DumpImage(fname, mNTiles, bCopyToScreen);
    }
    else
    {
      if (bWaitSignal)
        mPupil->RedrawWaitSignal();
      else
      mPupil->Redraw();
    }
    Stamp(FID());
  }
}

void ScreenDumper::Operate(Operator::Arg* op_arg)
{
  PreOperate(op_arg);
  DumpScreen();
  PostOperate(op_arg);
}

/**************************************************************************/
