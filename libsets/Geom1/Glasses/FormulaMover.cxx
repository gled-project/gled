// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

//__________________________________________________________________________
// FormulaMover
//
//

#include "FormulaMover.h"
#include <TFormula.h>

using namespace gled;

#include "FormulaMover.c7"

/**************************************************************************/

void FormulaMover::_init()
{
  mForX = mForY = mForZ = 0;
  mForPhi = mForTheta = mForEta = 0;
}

/**************************************************************************/

void FormulaMover::EmitPosChangedRay()
{
  static const Exc_t _eh("FormulaMover::EmitPosChangedRay ");
  std::cout << _eh <<"\n";
}

void FormulaMover::EmitRotChangedRay()
{
  static const Exc_t _eh("FormulaMover::EmitRotChangedRay ");
  std::cout << _eh <<"\n";
}

/**************************************************************************/

void FormulaMover::TimeTick(Double_t t, Double_t dt)
{
  printf("FormulaMover::TimeTick t=%lf, ft=%lf\n", t, dt);
}
