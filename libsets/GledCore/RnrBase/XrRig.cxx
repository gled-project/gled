// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "XrRig.h"

#include <TMath.h>

#include <cmath>

using namespace gled;

namespace
{
  ZPoint pos_of(const ZTrans& t) { return ZPoint(t.CM(0, 3), t.CM(1, 3), t.CM(2, 3)); }

  // The rotation taking the unit vector u to +y, about the axis u x y.
  ZTrans to_up(const ZPoint& u)
  {
    ZTrans t;
    const Double_t a[3] = { -u.z, 0, u.x }, s2 = a[0]*a[0] + a[2]*a[2], c = u.y;
    if (s2 < 1e-18) return t;
    const Double_t f = (1 - c) / s2;
    for (int i = 0; i < 3; ++i)
      for (int j = 0; j < 3; ++j)
        t.CM(i, j) = (i == j ? c : 0) + f * a[i] * a[j];
    t.CM(0, 1) -= a[2]; t.CM(0, 2) += a[1];
    t.CM(1, 0) += a[2]; t.CM(1, 2) -= a[0];
    t.CM(2, 0) -= a[1]; t.CM(2, 1) += a[0];
    return t;
  }
}

ZTrans XrRig::Translation(Double_t x, Double_t y, Double_t z)
{ ZTrans t; t.CM(0, 3) = x; t.CM(1, 3) = y; t.CM(2, 3) = z; return t; }

ZTrans XrRig::Scaling(Double_t s)
{ ZTrans t; for (int i = 0; i < 3; ++i) t.CM(i, i) = s; return t; }

ZTrans XrRig::Yaw(Double_t a)
{
  ZTrans t;
  t.CM(0, 0) =  std::cos(a); t.CM(0, 2) = std::sin(a);
  t.CM(2, 0) = -std::sin(a); t.CM(2, 2) = std::cos(a);
  return t;
}

Double_t XrRig::YawOf(const ZTrans& t)
{ return std::atan2(t.CM(0, 2) - t.CM(2, 0), t.CM(0, 0) + t.CM(2, 2)); }

//==============================================================================

XrRig::XrRig(Double_t metres_per_unit) :
  bRecenter(true), bRealign(false),
  fGrabMask(0), fDist0(1), fYaw0(0),
  fSpeed(0), fFlyAccel(3), bLevel(false)
{
  fW = fWStart = Scaling(metres_per_unit);
}

ZTrans XrRig::RefFromWorld() const
{
  ZTrans w(fW);
  return w * fLook;
}

Double_t XrRig::Scale() const
{
  return std::sqrt(fW.CM(0, 0)*fW.CM(0, 0) + fW.CM(1, 0)*fW.CM(1, 0) + fW.CM(2, 0)*fW.CM(2, 0));
}

void XrRig::Recenter()
{
  bRecenter = true;
  fSpeed    = 0;
}

void XrRig::Realign()
{
  bRealign = true;
  fSpeed   = 0;
}

void XrRig::Follow(const ZTrans& look, const ZTrans& head)
{
  const ZPoint h = pos_of(head);
  fLook = look;
  if (bRecenter)
  {
    fW        = Translation(h.x, h.y, h.z) * Yaw(YawOf(head)) * fWStart;
    fGrabMask = -1;
    bRecenter = bRealign = false;
    // The ridden frame's up, GL's +y, in world coordinates.
    ZTrans inv(fLook);
    inv.Invert();
    fUp.Set(0, 1, 0);
    inv.RotateIP(fUp);
    fUp.Normalize();
  }
  else if (bLevel)
  {
    // Undo the tilt about the ridden frame's place: the up of recentering stays up.
    ZPoint u(fUp), c = pos_of(fW);
    (fW * fLook).RotateIP(u);
    u.Normalize();
    fW = Translation(c.x, c.y, c.z) * to_up(u) * Translation(-c.x, -c.y, -c.z) * fW;
  }
  if (bRealign)
  {
    const Double_t a = YawOf(head) - YawOf(fW);
    fW        = Translation(h.x, 0, h.z) * Yaw(a) * Translation(-h.x, 0, -h.z) * fW;
    fGrabMask = -1;
    bRealign  = false;
  }
}

bool XrRig::Grab(const ZTrans* hands[2])
{
  const Int_t mask = (hands[0] ? 1 : 0) | (hands[1] ? 2 : 0);
  if (mask) fSpeed = 0;

  ZPoint   pl, pr, mid;
  Double_t dist = 0, ang = 0;
  if (mask == 3)
  {
    pl   = pos_of(*hands[0]);
    pr   = pos_of(*hands[1]);
    mid.Set((pl.x + pr.x) / 2, (pl.y + pr.y) / 2, (pl.z + pr.z) / 2);
    dist = pl.Distance(pr);
    ang  = std::atan2(pl.x - pr.x, pl.z - pr.z);
  }

  if (mask != fGrabMask)
  {
    // A hand took hold or let go: anchor at the current placement.
    fW0       = fW;
    fGrabMask = mask;
    if (mask == 1 || mask == 2)
    {
      const ZTrans &h = *hands[mask - 1];
      fMid0 = pos_of(h);
      fYaw0 = YawOf(h);
    }
    else if (mask == 3)
    {
      fMid0  = mid;
      fDist0 = TMath::Max(dist, 1e-3);
      fYaw0  = ang;
    }
  }
  else if (mask == 1 || mask == 2)
  {
    // The scene follows the hand's position and heading, and stays level.
    const ZTrans &h = *hands[mask - 1];
    fW = Translation(h.CM(0, 3), h.CM(1, 3), h.CM(2, 3)) * Yaw(YawOf(h) - fYaw0) *
         Translation(-fMid0.x, -fMid0.y, -fMid0.z) * fW0;
  }
  else if (mask == 3)
  {
    // Scale by the hands' distance, turn about the vertical, move with their midpoint.
    fW = Translation(mid.x, mid.y, mid.z) * Yaw(ang - fYaw0) * Scaling(dist / fDist0) *
         Translation(-fMid0.x, -fMid0.y, -fMid0.z) * fW0;
  }
  return mask != 0;
}

void XrRig::Fly(const ZTrans& head, const Float_t stick[4], Double_t dt)
{
  if (std::abs(stick[1]) > 0.15)
    fSpeed = TMath::Range(-50.0, 50.0, fSpeed + stick[1] * fFlyAccel * dt);
  else
    fSpeed *= std::exp(-dt / 0.4);

  // The world moves against the head, along the gaze and sideways, both kept
  // level; up and down only by the right stick.
  const Double_t f = fSpeed * dt, s = 2 * stick[0] * dt, u = 2 * stick[3] * dt;
  const Double_t a = YawOf(head), sa = std::sin(a), ca = std::cos(a);
  ZTrans move = Translation(f * sa - s * ca, u, f * ca + s * sa);
  if (std::abs(stick[2]) > 0.15)
  {
    const ZPoint h = pos_of(head);
    move = Translation(h.x, h.y, h.z) * Yaw(stick[2] * dt) * Translation(-h.x, -h.y, -h.z) * move;
  }
  fW = move * fW;
}
