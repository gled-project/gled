// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef Var1_VelocityVar_H
#define Var1_VelocityVar_H

#include <Gled/GledTypes.h>
#include <TObject.h>

namespace gled {

class VelocityVar : public TObject
{
public:
  enum DesireMode_e
  {
    DM_DesiredVelocity,
    DM_DesiredDeltaVelocity, // goes to DesiredVelocity when reached
    DM_DesiredDeltaValue,    // goes to DesiredVelocity 0 when reached

    // The following are composite ones. Should split them off? Next level control?
    DM_RandomRange, // Chooses delta, then keeps it for some time, goes back
    DM_RandomWalk
  };

protected:
  DesireMode_e fMode;
  Float_t      fDesire;

  Float_t      fValue;
  // Hmmh, do we need it here? Now the angular velocities are packed
  // in a vector of some sort. While this is ok calculations of the
  // analyitical mechanics stuff, it encumbers the connection.
  // Could:
  //   a) Have a reference to the vector value.
  //   b) Instantiate vector quantities where they are needed.
  //      This is also good, as they can be in any format there.

private:
  void _init();

protected:

public:
  VelocityVar();
  virtual ~VelocityVar();

#include "VelocityVar.h7"
  ClassDef(VelocityVar, 1);
}; // endclass VelocityVar

} // endnamespace gled

#endif
