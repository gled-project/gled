// A roller coaster to ride in a headset: a closed track with a chain lift, a
// drop, a vertical loop, a banked turn and a helix, under a sky, over tropical
// hills with water-filled craters.
//
// The track's centre line is a centripetal Catmull-Rom spline through key
// points, resampled into WSPoints about 2 m apart. Each segment takes the
// time its speed gives: 3 m/s on the chain and in the station, from energy
// after the crest, braked down on the way back. The points are banked so the
// felt force, acceleration minus gravity, presses into the seat. The cart
// rides with the up from the points; with flight, WSWalker banks it by the
// felt force as it goes instead.
//
// mpu:    metres per gled unit, as GLED_XR_SCALE (0.2 by default).
// flight: bank by WSWalker's flight mode instead of the points.
//
// vars: ZQueen* g_queen
// libs: Geom1

#include <glass_defines.h>
#include <gl_defines.h>

#include "sun_demos.C"
#include "eye.C"

#include <vector>

using namespace gled;

namespace
{
  struct CoasterKey { Double_t x, y, z; char sec; }; // sec: c chain, f free, b brake

  // The track in metres, z up, closed: the last key leads back to the first.
  const CoasterKey coaster_keys[] = {
    { -30,  0,  2, 'c' }, { -10,  0,  2, 'c' }, {   5,  0,  2.5, 'c' },
    {  15,  0,  7, 'c' }, {  35,  0, 24, 'c' }, {  46,  0, 30, 'c' },
    {  54,  0, 30.5, 'f' },
    {  64,  0, 21, 'f' }, {  74,  0,  8, 'f' }, {  86,  0,  3, 'f' },
    { 100,  0,  3, 'f' },
    // The vertical loop, radius 10, drifting 6 m sideways.
    { 112 + 10*0.7071, 0.75,  3 + 10 - 10*0.7071, 'f' },
    { 122,  1.5, 13, 'f' }, { 112 + 10*0.7071, 2.25, 13 + 10*0.7071, 'f' },
    { 112,  3, 23, 'f' }, { 112 - 10*0.7071, 3.75, 13 + 10*0.7071, 'f' },
    { 102,  4.5, 13, 'f' }, { 112 - 10*0.7071, 5.25, 13 - 10*0.7071, 'f' },
    { 114,  6,  3, 'f' }, { 128,  6,  3.5, 'f' },
    // A banked turn to the left, climbing.
    { 142,  9,  5, 'f' }, { 156, 18,  7, 'f' }, { 160, 30,  9, 'f' },
    { 154, 42, 11, 'f' }, { 140, 48, 12, 'f' }, { 110, 48, 12, 'f' },
    // A helix, one and a quarter turns down, radius 12 around (60, 36).
    {  80, 48, 11.5, 'f' }, {  60, 48, 11, 'f' }, {  51.5, 44.5, 10.4, 'f' },
    {  48, 36,  9.8, 'f' }, {  51.5, 27.5, 9.2, 'f' }, {  60, 24, 8.6, 'f' },
    {  68.5, 27.5, 8, 'f' }, {  72, 36,  7.4, 'f' }, {  68.5, 44.5, 6.8, 'f' },
    {  60, 48,  6.2, 'f' }, {  51.5, 44.5, 5.6, 'f' }, {  48, 36,  5, 'f' },
    // Back under the lift hill, the brakes, and round into the station.
    {  46, 26,  4, 'f' }, {  38, 18,  3, 'f' }, {  20, 16,  3, 'b' },
    {   0, 16,  3, 'b' }, { -20, 16,  2.5, 'c' }, { -30, 16, 2.5, 'c' },
    { -35.66, 13.66, 2.2, 'c' }, { -38, 8, 2, 'c' }, { -35.66, 2.34, 2, 'c' }
  };

  struct TrackPoint { ZPoint pos, x, z; char sec; Double_t v, dt; };

  // Centripetal Catmull-Rom between p1 and p2 at u in [0, 1].
  ZPoint catmull_rom(const ZPoint& p0, const ZPoint& p1, const ZPoint& p2,
                     const ZPoint& p3, Double_t u)
  {
    const Double_t t0 = 0, t1 = t0 + std::sqrt((p1 - p0).Mag()),
                   t2 = t1 + std::sqrt((p2 - p1).Mag()), t3 = t2 + std::sqrt((p3 - p2).Mag());
    const Double_t t = t1 + u * (t2 - t1);
    const ZPoint a1 = p0 * ((t1 - t) / (t1 - t0)) + p1 * ((t - t0) / (t1 - t0));
    const ZPoint a2 = p1 * ((t2 - t) / (t2 - t1)) + p2 * ((t - t1) / (t2 - t1));
    const ZPoint a3 = p2 * ((t3 - t) / (t3 - t2)) + p3 * ((t - t2) / (t3 - t2));
    const ZPoint b1 = a1 * ((t2 - t) / (t2 - t0)) + a2 * ((t - t0) / (t2 - t0));
    const ZPoint b2 = a2 * ((t3 - t) / (t3 - t1)) + a3 * ((t - t1) / (t3 - t1));
    return b1 * ((t2 - t) / (t2 - t1)) + b2 * ((t - t1) / (t2 - t1));
  }

  // The closed centre line, every ds metres, with tangents, sections and speeds.
  std::vector<TrackPoint> coaster_track(Double_t ds, Double_t g)
  {
    const int nk = sizeof(coaster_keys) / sizeof(coaster_keys[0]);
    auto key = [&](int i) { const CoasterKey& k = coaster_keys[(i % nk + nk) % nk];
                            return ZPoint(k.x, k.y, k.z); };

    // Dense samples, then points at equal arc lengths.
    std::vector<ZPoint> dense;
    std::vector<char>   dsec;
    for (int k = 0; k < nk; ++k)
      for (int j = 0; j < 64; ++j)
      {
        dense.push_back(catmull_rom(key(k - 1), key(k), key(k + 1), key(k + 2), j / 64.0));
        dsec.push_back(coaster_keys[k].sec);
      }
    std::vector<TrackPoint> tp;
    Double_t len = 0, next = 0;
    const int nd = dense.size();
    for (int i = 0; i < nd; ++i)
    {
      if (len >= next)
      {
        TrackPoint p;
        p.pos = dense[i];
        p.x   = dense[(i + 1) % nd] - dense[(i - 1 + nd) % nd];
        p.x.Normalize();
        p.sec = dsec[i];
        tp.push_back(p);
        next += ds;
      }
      len += (dense[(i + 1) % nd] - dense[i]).Mag();
    }

    // Speeds: the chain's 3 m/s, energy below the crest, braking linearly.
    const int n = tp.size();
    Double_t z_top = 0;
    for (auto& p : tp) if (p.sec == 'c') z_top = TMath::Max(z_top, p.pos.z);
    int b0 = -1, b1 = -1;
    for (int i = 0; i < n; ++i)
      if (tp[i].sec == 'b') { if (b0 < 0) b0 = i; b1 = i; }
    for (int i = 0; i < n; ++i)
    {
      const Double_t vf = std::sqrt(9 + 2 * g * TMath::Max(0.0, z_top - tp[i].pos.z));
      if      (tp[i].sec == 'c') tp[i].v = 3;
      else if (tp[i].sec == 'f') tp[i].v = vf;
      else
      {
        const Double_t vb = std::sqrt(9 + 2 * g * TMath::Max(0.0, z_top - tp[b0].pos.z));
        tp[i].v = vb + (3 - vb) * (i - b0) / Double_t(b1 - b0 + 1);
      }
    }
    for (int i = 0; i < n; ++i)
    {
      const TrackPoint &a = tp[i], &b = tp[(i + 1) % n];
      tp[i].dt = (b.pos - a.pos).Mag() / (0.5 * (a.v + b.v));
    }
    return tp;
  }

  ZTrans frame(const ZPoint& pos, const ZPoint& x, const ZPoint& z)
  {
    ZTrans t;
    t.SetBaseVec(1, x);
    t.SetBaseVec(2, z.Cross(x));
    t.SetBaseVec(3, z);
    t.SetBaseVec(4, pos);
    return t;
  }

  struct Crater { Double_t x, y, r, depth; };

  // Ground height in metres: rolling hills, mountains towards the edges, and
  // craters whose floors below the water level fill with water; flattened
  // to at most 0.5 m near the track.
  Double_t coaster_ground(Double_t x, Double_t y, Double_t d_track, Double_t water)
  {
    static const Crater craters[] = {
      { -60, 60, 25, 9 }, { 20, 92, 18, 8 }, { 150, 98, 30, 10 },
      { 200, -40, 22, 8 }, { -40, -60, 20, 8 }, { 90, -52, 16, 7 }
    };
    Double_t h = 4 + 3 * std::sin(x / 37) * std::cos(y / 29) + 2 * std::sin((x + y) / 23);
    const Double_t rc = std::hypot(x - 60, y - 20);
    if (rc > 130) h += 14 * TMath::Min(1.0, (rc - 130) / 60);
    for (const Crater& c : craters)
    {
      const Double_t r = std::hypot(x - c.x, y - c.y) / c.r;
      if (r < 1) h -= c.depth * (1 - r * r);
      h += 1.5 * std::exp(-std::pow((r - 1) / 0.25, 2));
    }
    h = TMath::Max(h, water);
    const Double_t f = TMath::Min(1.0, TMath::Max(0.0, (d_track - 6) / 20));
    return f * h + (1 - f) * TMath::Min(h, 0.5);
  }

  WSSeed* rail_seed(ZNode* parent, const char* name, Float_t radius, const ZColor& col)
  {
    WSSeed* s = new WSSeed(name);
    s->SetFat(true);
    s->SetTLevel(3);
    s->SetPLevel(8);
    s->SetColor(col.r(), col.g(), col.b());
    s->SetRnrElements(false);
    g_queen->CheckIn(s);
    parent->Add(s);
    return s;
  }

  void add_point(WSSeed* s, const ZTrans& t, Float_t w, Float_t dt)
  {
    WSPoint* p = new WSPoint();
    p->SetTrans(t);
    p->SetW(w);
    p->SetStretch(dt);
    g_queen->CheckIn(p);
    s->Add(p);
  }
}

void coaster(Double_t mpu=0.2, Bool_t flight=false)
{
  ASSERT_MACRO(sun_demos);
  Gled::theOne->AssertLibSet("Geom1");

  const Double_t m = 1 / mpu;            // gled units per metre
  const Double_t g = 9.81;

  Scene* scene = new Scene("Coaster Scene");
  g_queen->CheckIn(scene);
  g_queen->Add(scene);
  g_scene = scene;

  // Light, sky and ground.
  CREATE_ADD_GLASS(sun, Lamp, scene, "Sun", 0);
  sun->SetPos(60*m, -100*m, 300*m);
  sun->SetDiffuse(1, 0.95, 0.85);
  sun->SetAmbient(0.35, 0.35, 0.4);
  sun->SetLampScale(0);
  scene->GetGlobLamps()->Add(sun);

  CREATE_ADD_GLASS(sky_img, ZImage, scene, "SkyGradient", 0);
  sky_img->SetFile("sky_gradient.png");
  sky_img->Load();
  sky_img->SetEnvMode(GL_REPLACE);
  sky_img->SetMagFilter(GL_LINEAR);
  sky_img->SetMinFilter(GL_LINEAR);

  // Unlit, seen from inside, around the whole ride and inside the headset's
  // far plane of 1000 m.
  CREATE_ADD_GLASS(sky_mod, ZGlLightModel, scene, "SkyLightModel", 0);
  sky_mod->SetLightModelOp(ZRnrModBase::O_Off);
  sky_mod->SetFaceCullOp(ZRnrModBase::O_Off);
  sky_mod->SetShadeModelOp(ZRnrModBase::O_On);  // filled back faces too
  CREATE_ADD_GLASS(sky, SMorph, scene, "Sky", 0);
  sky->SetPos(60*m, 20*m, 0);
  sky->RotateLF(1, 3, TMath::PiOver2());
  sky->SetScale(600*m);
  sky->SetTLevel(24); sky->SetPLevel(48);
  sky->SetTexture(sky_img);
  sky->SetRnrMod(sky_mod);

  // The track: the centre line the cart rides, hidden, and what is drawn.
  std::vector<TrackPoint> tp = coaster_track(2, g);
  const int n = tp.size();

  // The ground, 400 m square in cells of 2 m, coloured by height.
  CREATE_ADD_GLASS(palette, RGBAPalette, scene, "Tropical", 0);
  palette->SetMarksFromPOVFile("coaster_tropical.pov");
  {
    const int    nc = 201;
    const Double_t x0 = 60 - 200, y0 = 20 - 200, dc = 2, water = -2;
    std::vector<Float_t> hf(nc * nc);
    for (int iy = 0; iy < nc; ++iy)
      for (int ix = 0; ix < nc; ++ix)
      {
        const Double_t x = x0 + ix * dc, y = y0 + iy * dc;
        Double_t d2 = 1e12;
        for (const TrackPoint& p : tp)
          d2 = TMath::Min(d2, (p.pos.x - x) * (p.pos.x - x) + (p.pos.y - y) * (p.pos.y - y));
        hf[iy * nc + ix] = coaster_ground(x, y, std::sqrt(d2), water) * m;
      }
    CREATE_ADD_GLASS(ground, RectTerrain, scene, "Ground", 0);
    ground->SetFromArray(nc, nc, &hf[0]);
    ground->SetDx(dc*m); ground->SetDy(dc*m);
    ground->SetPos(x0*m, y0*m, 0);
    ground->SetRibbon(palette);
    ground->SetRnrMode(RectTerrain::RM_SmoothTring);
  }

  CREATE_ADD_GLASS(line, WSSeed, scene, "CentreLine", 0);
  line->SetRnrSelf(false);
  line->SetRnrElements(false);
  line->SetUpFromPoints(true);
  for (int i = 0; i <= n; ++i)
  {
    const TrackPoint& p = tp[i % n];
    ZPoint z(0, 0, 1);
    z -= p.x * z.Dot(p.x);
    z.Normalize();
    add_point(line, frame(p.pos * m, p.x, z), 0.1, i < n ? p.dt : 1);
  }

  // Banking: the felt force a - g, normal to the tangent, from the centre
  // line's own motion, averaged across each point. Where it is weak or pulls
  // out of the seat, as over a crest, the bank stays: airtime, not a roll.
  std::vector<ZPoint> up(n);
  Double_t time = 0;
  for (int i = 0; i < n; ++i)
  {
    ZPoint pos, vel, a1, a2;
    line->MotionAtTime(TMath::Max(0.0, time - 1e-4), pos, vel, a1);
    line->MotionAtTime(time + 1e-4, pos, vel, a2);
    ZPoint f = (a1 + a2) * (0.5 / m);
    f.z += g;
    f -= tp[i].x * f.Dot(tp[i].x);
    ZPoint prev = i ? up[i - 1] : ZPoint(0, 0, 1);
    prev -= tp[i].x * prev.Dot(tp[i].x);
    prev.Normalize();
    up[i] = f.Dot(prev) > 0.3 * g ? f * (1 / f.Mag()) : prev;
    time += tp[i].dt;
  }
  for (int pass = 0; pass < 3; ++pass)
  {
    std::vector<ZPoint> s(n);
    for (int i = 0; i < n; ++i)
    {
      s[i] = up[(i - 1 + n) % n] + up[i] * 2.0 + up[(i + 1) % n];
      s[i] -= tp[i].x * s[i].Dot(tp[i].x);
      s[i].Normalize();
    }
    up = s;
  }
  for (int i = 0; i < n; ++i) tp[i].z = up[i];

  int i = 0;
  for (auto it = line->begin(); it != line->end(); ++it, ++i)
  {
    WSPoint* p = dynamic_cast<WSPoint*>(it->fLens);
    if (p) p->SetTrans(frame(tp[i % n].pos * m, tp[i % n].x, tp[i % n].z));
  }

  ZColor steel(0.75, 0.75, 0.8), red(0.8, 0.15, 0.1);
  WSSeed* rail_l = rail_seed(scene, "RailLeft",  0.06*m, steel);
  WSSeed* rail_r = rail_seed(scene, "RailRight", 0.06*m, steel);
  WSSeed* spine  = rail_seed(scene, "Spine",     0.18*m, red);
  for (int k = 0; k <= n; ++k)
  {
    const TrackPoint& p = tp[k % n];
    const ZPoint y = p.z.Cross(p.x);
    add_point(rail_l, frame((p.pos + y * 0.6) * m, p.x, p.z), 0.06*m, 1);
    add_point(rail_r, frame((p.pos - y * 0.6) * m, p.x, p.z), 0.06*m, 1);
    add_point(spine,  frame((p.pos - p.z * 0.35) * m, p.x, p.z), 0.18*m, 1);
  }

  CREATE_ADD_GLASS(ties, ZNode, scene, "Ties", 0);
  CREATE_ADD_GLASS(pillars, ZNode, scene, "Pillars", 0);
  for (int k = 0; k < n; ++k)
  {
    const TrackPoint& p = tp[k];
    if (k % 2 == 0)
    {
      CREATE_ADD_GLASS(tie, Box, ties, GForm("Tie %d", k), 0);
      tie->SetTrans(frame((p.pos - p.z * 0.2) * m, p.x, p.z));
      tie->SetABC(0.25*m, 1.4*m, 0.12*m);
      tie->SetColor(0.3, 0.3, 0.35);
    }
    if (k % 5 == 0 && p.pos.z > 3 && p.z.z > 0.5)
    {
      const Double_t h = p.pos.z - 0.5;
      CREATE_ADD_GLASS(pil, Box, pillars, GForm("Pillar %d", k), 0);
      pil->SetPos(p.pos.x*m, p.pos.y*m, 0.5*h*m);
      pil->SetABC(0.4*m, 0.4*m, h*m);
      pil->SetColor(0.85, 0.85, 0.8);
    }
  }

  // The cart, and the seat the camera rides.
  CREATE_ADD_GLASS(car, ZNode, scene, "Car", 0);
  car->SetTrans(frame(tp[0].pos * m, tp[0].x, tp[0].z));
  // An open half shell, textured and lit from both sides.
  CREATE_ADD_GLASS(cart_img, ZImage, scene, "CartTexture", 0);
  cart_img->SetFile("checker_4.png");
  cart_img->Load();
  CREATE_ADD_GLASS(cart_mod, ZGlLightModel, scene, "CartLightModel", 0);
  cart_mod->SetLightModelOp(ZRnrModBase::O_On);
  cart_mod->SetLiMoTwoSide(true);
  cart_mod->SetShadeModelOp(ZRnrModBase::O_On);
  cart_mod->SetFaceCullOp(ZRnrModBase::O_Off);
  CREATE_ADD_GLASS(body, SMorph, car, "Body", 0);
  body->SetPos(0, 0, 0.6*m);
  body->SetScales(1.2*m, 0.75*m, 0.45*m);
  body->SetPhiMean(0.75); body->SetPhiRange(0.5);
  body->SetTLevel(16); body->SetPLevel(16);
  body->SetColor(1, 0.3, 0.2);
  body->SetTexture(cart_img);
  body->SetTexXC(2);
  body->SetRnrMod(cart_mod);
  CREATE_ADD_GLASS(seat, ZNode, car, "Seat", 0);
  seat->SetPos(-0.2*m, 0, 1.25*m);

  CREATE_ADD_GLASS(etor, Eventor, scene, "Eventor", 0);
  etor->SetInterBeatMS(gSystem->Getenv("GLED_XR") ? 11 : 20);
  CREATE_ADD_GLASS(tmaker, TimeMaker, etor, "TimeMaker", 0);
  CREATE_ATT_GLASS(walker, WSWalker, tmaker, AddClient, "Walker", 0);
  walker->SetSeed(line);
  walker->SetNode(car);
  walker->SetLoop(true);
  walker->SetFlight(flight);
  walker->SetGravity(g*m);

  printf("coaster: %d points, %.0f m, a lap in %.0f s.\n", n, 2.0*n, time);

  eye();
  g_pupil->SetCameraBase(seat);
  g_pupil->SetXrTilt(PupilInfo::XT_Ride);
  g_pupil->SetNearClip(0.1*m);
  g_pupil->SetFarClip(1500*m);
  g_pupil->SetClearColor(0.6, 0.72, 0.9);
  // The camera sits on the seat once the pupil exists.
  TTimer::SingleShot(3000, "gled::PupilInfo", g_pupil, "EmitCameraHomeRay()");

  etor->Start();
}
