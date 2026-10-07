// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "Audio1.h"

#include <AL/alut.h>

using namespace gled;


using namespace Audio1;

void Audio1::CheckAlError(const Exc_t& eh)
{
  ALenum err = alGetError();

  if (err != AL_NO_ERROR)
  {
    switch (err)
    {
      case AL_INVALID_NAME:
	ISerr(eh + "Invalid name parameter.");
	break;
      case AL_INVALID_ENUM:
	ISerr(eh + "Invalid parameter.");
	break;
      case AL_INVALID_VALUE:
	ISerr(eh + "Invalid enum parameter value.");
	break;
      case AL_INVALID_OPERATION:
	ISerr(eh + "Illegal call.");
	break;
      case AL_OUT_OF_MEMORY:
	ISerr(eh + "Unable to allocate memory.");
	break;
      default:
	ISerr(eh + "Unknown error.");
	break;
    }
  }
}
