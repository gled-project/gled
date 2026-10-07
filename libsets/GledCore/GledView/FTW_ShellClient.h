// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef GledCore_FTW_ShellClient_H
#define GledCore_FTW_ShellClient_H

namespace gled {

class FTW_Shell;

class FTW_ShellClient
{
protected:
  FTW_Shell*    mShell;

public:
  FTW_ShellClient(FTW_Shell* s);

  FTW_Shell* GetShell() { return mShell; }
};

} // endnamespace gled

#endif
