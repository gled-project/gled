// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef Var1_Planetes_H
#define Var1_Planetes_H

#include <Glasses/ZNode.h>
#include <Stones/ZColor.h>

namespace gled {

class HTriMesh;

class Planetes : public ZNode
{
  MAC_RNR_FRIENDS(Planetes);

private:
  void _init();

protected:
  ZLink<HTriMesh>   mMesh;      //  X{GE} L{}
  ZColor            mColor;     //  X{GSPR} 7 ColorButt()

  Int_t             mDrawLevel; // X{GE} 7 Value(-range=>[0, 100, 1])

public:
  Planetes(const Text_t* n="Planetes", const Text_t* t=0);
  virtual ~Planetes();

  void SetMesh(HTriMesh* mesh);
  void SetDrawLevel(Int_t l);

#include "Planetes.h7"
  ClassDef(Planetes, 1);
}; // endclass Planetes

} // endnamespace gled

#endif
