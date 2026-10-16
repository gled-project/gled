// Checks XrRig, the headset's scene placement, without a headset; see test/README.md.
#include <Gled/GledNS.h>
#include <RnrBase/XrRig.h>

#include <cmath>

using namespace gled;

namespace
{
  int n_failed = 0;

  void check(const char* what, Double_t dev, Double_t tol=1e-9)
  {
    printf("xr_rig: %-52s %s (%.2g)\n", what, dev < tol ? "ok" : "FAILED", dev);
    if ( ! (dev < tol)) ++n_failed;
  }

  ZPoint apply(const ZTrans& t, const ZPoint& p)
  { ZPoint q(p); t.MultiplyIP(q); return q; }

  ZPoint rotate(const ZTrans& t, const ZPoint& p)
  { ZPoint q(p); t.RotateIP(q); return q; }

  Double_t dist(const ZPoint& a, const ZPoint& b) { return a.Distance(b); }

  // The view of a ridden frame: gled's axes (x forward, y left, z up) to GL's eye axes.
  ZTrans look_of(const ZTrans& ride)
  {
    ZTrans r(ride);
    r.Invert();
    ZTrans axes;
    axes.CM(0, 0) = 0; axes.CM(0, 1) = -1;
    axes.CM(1, 1) = 0; axes.CM(1, 2) =  1;
    axes.CM(2, 2) = 0; axes.CM(2, 0) = -1;
    return axes * r;
  }

  ZTrans pose(Double_t x, Double_t y, Double_t z, Double_t yaw)
  {
    ZTrans t = XrRig::Yaw(yaw);
    t.CM(0, 3) = x; t.CM(1, 3) = y; t.CM(2, 3) = z;
    return t;
  }
}

void xr_rig()
{
  XrRig rig(0.2);

  // A ridden frame somewhere in the world, turned and tilted.
  ZTrans ride;
  ride.RotateLF(1, 2, 0.7);
  ride.RotateLF(1, 3, 0.2);
  ride.SetBaseVec(4, ZPoint(10, 5, 2));
  const ZPoint ride_pos(10, 5, 2);

  // Recentering: the frame under the head, its forward along the gaze.
  ZTrans head = pose(0.3, 1.6, -0.2, 0.5);
  rig.Follow(look_of(ride), head);
  check("recenter: the frame under the head",
        dist(apply(rig.RefFromWorld(), ride_pos), ZPoint(0.3, 0, -0.2)));
  ZPoint fwd = rotate(rig.RefFromWorld(), ZPoint(ride.CM(0, 0), ride.CM(1, 0), ride.CM(2, 0)));
  fwd.y = 0;
  fwd.Normalize();
  check("recenter: forward along the gaze", dist(fwd, ZPoint(-std::sin(0.5), 0, -std::cos(0.5))));
  check("recenter: the scale", std::abs(rig.Scale() - 0.2));

  // A turn in place, with the head moved away from the frame: the head stays
  // over the same world point.
  head = pose(0.6, 1.5, 0.1, 0.5);
  ZTrans inv = rig.RefFromWorld();
  inv.Invert();
  const ZPoint under_head = apply(inv, ZPoint(0.6, 1.5, 0.1));
  for (int i = 0; i < 10; ++i)
  {
    ride.RotateLF(1, 2, 0.05);
    rig.Follow(look_of(ride), head);
  }
  ZPoint h = apply(rig.RefFromWorld(), under_head);
  check("turn: the head stays over its world point", std::hypot(h.x - 0.6, h.z - 0.1));

  // One hand carries the scene rigidly and keeps it level.
  const ZTrans h1 = pose(0.2, 1.2, -0.4, 0.1), h2 = pose(0.35, 1.3, -0.3, 0.4);
  const ZTrans* hands[2] = { &h1, 0 };
  rig.Grab(hands);
  inv = rig.RefFromWorld();
  inv.Invert();
  const ZPoint under_hand = apply(inv, ZPoint(0.2, 1.2, -0.4));
  hands[0] = &h2;
  rig.Grab(hands);
  check("grab: the scene follows the hand",
        dist(apply(rig.RefFromWorld(), under_hand), ZPoint(0.35, 1.3, -0.3)));
  // The ridden frame's up was the reference space's up since recentering.
  ZPoint up = rotate(rig.RefFromWorld(), ZPoint(ride.CM(0, 2), ride.CM(1, 2), ride.CM(2, 2)));
  up.Normalize();
  check("grab: the scene stays level", dist(up, ZPoint(0, 1, 0)));

  // Two hands scale by their distance.
  const ZTrans l1 = pose(-0.2, 1.2, -0.4, 0), r1 = pose(0.2, 1.2, -0.4, 0);
  const ZTrans l2 = pose(-0.4, 1.2, -0.4, 0), r2 = pose(0.4, 1.2, -0.4, 0);
  const Double_t s0 = rig.Scale();
  const ZTrans* two[2] = { &l1, &r1 };
  rig.Grab(two);
  two[0] = &l2; two[1] = &r2;
  rig.Grab(two);
  check("two hands: twice the distance, twice the scale", std::abs(rig.Scale() - 2 * s0));

  printf("xr_rig: %s\n", n_failed ? "FAILED" : "all passed");
}
