// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef GledCore_XrRig_H
#define GledCore_XrRig_H

#include <Stones/ZTrans.h>

namespace gled {

// The placement of a gled scene in a headset's reference space (metres, +y up),
// and the interactions that change it: recentering, turns pivoting at the
// head, grabbing with one or two hands, and flying. No OpenXR and no GL, so
// test/regress/xr_rig.C can check it without a headset.
class XrRig
{
  ZTrans    fW, fWStart;   // Ridden frame's GL eye coordinates, gled units, to metres.
  ZTrans    fLook, fLookPrev;
  bool      bLookPrev, bRecenter;

  Int_t     fGrabMask;     // Hands holding: 1 left, 2 right; -1 to re-anchor.
  ZTrans    fW0;           // fW when the hands took hold,
  ZPoint    fMid0;         // and their position or midpoint, distance and heading.
  Double_t  fDist0, fYaw0;

  Double_t  fSpeed;        // Flying along the gaze, in metres per second.

public:
  Double_t  fFlyAccel;     // Of a fully pushed stick, in metres per second squared.

  XrRig(Double_t metres_per_unit=0.2);

  // Once per frame, before rendering. look: GL eye coordinates of the ridden
  // frame from gled's world; head: the head's pose in the reference space.
  void Follow(const ZTrans& look, const ZTrans& head);

  // At the next Follow: the ridden frame under the head, facing the gaze.
  void Recenter();

  // Hands that hold, in the reference space, 0 for one that does not.
  // Returns whether any holds.
  bool Grab(const ZTrans* hands[2]);

  // For scenes without a spirit to move the camera. Sticks: left x, y, right
  // x, y. Left y speeds up along the gaze, left x strafes, right x turns,
  // right y goes down when pushed.
  void Fly(const ZTrans& head, const Float_t stick[4], Double_t dt);

  // Reference-space coordinates of a point in gled's world.
  ZTrans   RefFromWorld() const;
  Double_t Scale() const;

  static ZTrans   Translation(Double_t x, Double_t y, Double_t z);
  static ZTrans   Scaling(Double_t s);
  static ZTrans   Yaw(Double_t a);
  // The turn of a rotation about the vertical, stable however a hand is held.
  static Double_t YawOf(const ZTrans& t);
};

} // endnamespace gled

#endif
