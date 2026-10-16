// Checks WSSeed's motion along its points, the up taken from them, and
// WSWalker's flight banking, without a display; see test/README.md.
#include <Gled/GledNS.h>

#include "sun_demos.C"

#include <cmath>

using namespace gled;

namespace
{
  int n_failed = 0;

  void check(const char* what, Double_t dev, Double_t tol)
  {
    printf("ws_motion: %-52s %s (%.2g)\n", what, dev < tol ? "ok" : "FAILED", dev);
    if ( ! (dev < tol)) ++n_failed;
  }

  ZPoint col(const ZTrans& t, int i) { return ZPoint(t.CM(0, i), t.CM(1, i), t.CM(2, i)); }

  // A horizontal circle of n points, banked by bank(i) about the tangent,
  // each segment taking dur(i) seconds.
  WSSeed* make_circle(const char* name, int n, Double_t r,
                      Double_t (*bank)(int), Double_t (*dur)(int))
  {
    WSSeed* seed = new WSSeed(name);
    g_queen->CheckIn(seed);
    for (int i = 0; i <= n; ++i)
    {
      const Double_t phi = 2 * TMath::Pi() * i / n, b = bank(i);
      const ZPoint x(-std::sin(phi), std::cos(phi), 0), in(-std::cos(phi), -std::sin(phi), 0);
      const ZPoint z = ZPoint(0, 0, 1) * std::cos(b) + in * std::sin(b);
      ZTrans t;
      t.SetBaseVec(1, x);
      t.SetBaseVec(2, z.Cross(x));
      t.SetBaseVec(3, z);
      t.SetBaseVec(4, ZPoint(r * std::cos(phi), r * std::sin(phi), 0));
      WSPoint* p = new WSPoint(GForm("%s %d", name, i));
      p->SetTrans(t);
      p->SetStretch(dur(i));
      g_queen->CheckIn(p);
      seed->Add(p);
    }
    return seed;
  }

  Double_t no_bank(int)    { return 0; }
  Double_t wavy_bank(int i) { return 0.4 * std::sin(0.9 * i); }
  Double_t even_dur(int)   { return 0.5; }
  Double_t uneven_dur(int i) { return 0.3 + 0.05 * (i % 4); }
}

void ws_motion()
{
  ASSERT_MACRO(sun_demos);
  Gled::theOne->AssertLibSet("Geom1");

  // Velocity and acceleration against finite differences of the position.
  WSSeed* s1 = make_circle("Uneven", 12, 5, wavy_bank, uneven_dur);
  Double_t dv = 0, da = 0;
  for (Double_t t = 0.11; t < 3.5; t += 0.37)
  {
    const Double_t h = 1e-4;
    ZPoint p0, v0, a0, pm, vm, am, pp, vp, ap;
    s1->MotionAtTime(t, p0, v0, a0);
    s1->MotionAtTime(t - h, pm, vm, am);
    s1->MotionAtTime(t + h, pp, vp, ap);
    dv = TMath::Max(dv, ((pp - pm) * (1 / (2 * h)) - v0).Mag() / v0.Mag());
    da = TMath::Max(da, ((vp - vm) * (1 / (2 * h)) - a0).Mag() / a0.Mag());
  }
  check("motion: velocity is the position's derivative", dv, 1e-6);
  check("motion: acceleration is the velocity's derivative", da, 1e-6);

  // The up from the points: at a point its z axis, between them normal to the tangent.
  s1->SetUpFromPoints(true);
  ZTrans lcf;
  Double_t dz = 0, dn = 0, time = 0;
  for (int i = 0; i < 12; ++i)
  {
    s1->TransAtTime(lcf, time + 1e-9, false, true);
    WSPoint* p = (WSPoint*) s1->FindLensByPath(GForm("Uneven %d", i));
    if (p) dz = TMath::Max(dz, (col(lcf, 2) - col(p->RefTrans(), 2)).Mag());
    s1->TransAtTime(lcf, time + 0.5 * uneven_dur(i), false, false);
    dn = TMath::Max(dn, std::abs(col(lcf, 2).Dot(col(lcf, 0))) + std::abs(col(lcf, 2).Mag() - 1));
    time += uneven_dur(i);
  }
  check("points' up: the point's z axis at the point", dz, 1e-6);
  check("points' up: unit and normal to the tangent between", dn, 1e-9);

  // Flight on a level circle: the bank of a coordinated turn, tan b = v^2 / (r g).
  const Double_t r = 20, v = 14, g = 9.81;
  const int n = 36;
  struct Dur { static Double_t f(int) { return 2 * TMath::Pi() * 20 / 36 / 14.0; } };
  WSSeed* s2 = make_circle("Level", n, r, no_bank, Dur::f);
  ZNode* car = new ZNode("Car");
  g_queen->CheckIn(car);
  WSWalker* w = new WSWalker("Walker");
  g_queen->CheckIn(w);
  w->SetSeed(s2);
  w->SetNode(car);
  w->SetFlight(true);
  w->SetGravity(g);
  w->SetFlightTau(0);
  Double_t db = 0;
  const Double_t expect = std::atan(v * v / (r * g));
  for (Double_t t = 0.05; t < 6; t += 0.29)
  {
    w->SetTime(t);
    const ZPoint up = col(car->RefTrans(), 2);
    const ZPoint pos = col(car->RefTrans(), 3);
    const ZPoint in = ZPoint(-pos.x, -pos.y, 0) * (1 / std::hypot(pos.x, pos.y));
    db = TMath::Max(db, std::abs(std::atan2(up.Dot(in), up.z) - expect));
  }
  check("flight: the bank of a coordinated turn, in degrees", db * 180 / TMath::Pi(), 1);

  printf("ws_motion: %s\n", n_failed ? "FAILED" : "all passed");
}
