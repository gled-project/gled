// A roller coaster to ride in a headset: a closed track with a chain lift, a
// drop, a vertical loop, a banked turn and a helix, in a tropical saddle
// between hills, with crater lakes, a sea at the valley ends, shadows, a sun
// and three kinds of clouds.
//
// The track's centre line is a centripetal Catmull-Rom spline through key
// points, resampled into WSPoints about 2 m apart. Each segment takes the
// time its speed gives: 3 m/s on the chain and in the station, from energy
// after the crest, braked down on the way back. The points are banked so the
// felt force, acceleration minus gravity, presses into the seat. The cart
// rides with the up from the points; with flight, WSWalker banks it by the
// felt force as it goes instead. The seat is the pupil's camera base.
//
// Everything is laid out in metres around the middle of the ride and scaled
// by mpu into gled units.
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

#include <TRandom3.h>

#include <vector>

using namespace gled;

namespace
{
  const ZPoint coaster_centre(60, 20, 0);   // the middle of the ride, in metres

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
  std::vector<TrackPoint> coaster_centre_line(Double_t ds, Double_t g)
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
  const Crater coaster_craters[] = {
    { 20, 112, 22, 10 }, { 115, 118, 18, 9 }, { -35, -85, 25, 10 },
    { 145, -72, 20, 9 }, { -85, 75, 16, 8 }
  };
  const Double_t coaster_sea = -6;

  // Ground height in metres before the track: a saddle with hills across the
  // track and valleys along it, bumps, and the craters' bowls and rims.
  Double_t coaster_land(Double_t x, Double_t y)
  {
    const Double_t u = (x - coaster_centre.x) / 180, v = (y - coaster_centre.y) / 140;
    Double_t h = 40 * v * v - 18 * u * u + 6 * std::sin(x / 41) * std::cos(y / 33)
               + 4 * std::sin((x + 2 * y) / 57) + 2.5 * std::sin(x / 13 + y / 17);
    for (const Crater& c : coaster_craters)
    {
      const Double_t r = std::hypot(x - c.x, y - c.y) / c.r;
      if (r < 1) h -= c.depth * (1 - r * r);
      h += 2 * std::exp(-std::pow((r - 1) / 0.25, 2));
    }
    return h;
  }

  // The ground: the land above the sea, kept between 0 and 0.5 m near the track.
  Double_t coaster_ground(Double_t x, Double_t y, Double_t d_track)
  {
    const Double_t h = TMath::Max(coaster_land(x, y), coaster_sea);
    const Double_t f = TMath::Min(1.0, TMath::Max(0.0, (d_track - 6) / 20));
    return f * h + (1 - f) * TMath::Min(TMath::Max(h, 0.0), 0.5);
  }

  // A crater's lake: half a metre below the lowest point of its rim, if its
  // floor is lower; -1e9 if it holds none.
  Double_t coaster_lake(const Crater& c)
  {
    Double_t rim = 1e9;
    for (int k = 0; k < 64; ++k)
    {
      const Double_t phi = k * TMath::TwoPi() / 64;
      rim = TMath::Min(rim, coaster_land(c.x + 1.1 * c.r * std::cos(phi), c.y + 1.1 * c.r * std::sin(phi)));
    }
    const Double_t level = rim - 0.5;
    return coaster_land(c.x, c.y) < level ? level : -1e9;
  }

  // Heights in metres on a square grid, bilinear.
  struct HeightGrid
  {
    Double_t x0, y0, dc; int nc; std::vector<Double_t> h;
    Double_t at(Double_t x, Double_t y) const
    {
      const Double_t fx = TMath::Max(0.0, TMath::Min(nc - 1.001, (x - x0) / dc));
      const Double_t fy = TMath::Max(0.0, TMath::Min(nc - 1.001, (y - y0) / dc));
      const int ix = int(fx), iy = int(fy);
      const Double_t ax = fx - ix, ay = fy - iy;
      const Double_t* r0 = &h[iy * nc + ix];
      const Double_t* r1 = r0 + nc;
      return (1 - ay) * ((1 - ax) * r0[0] + ax * r0[1]) + ay * ((1 - ax) * r1[0] + ax * r1[1]);
    }
  };

  // Cloud puffs in metres: cumulus 0, altocumulus 1, cirrus 2.
  struct Puff { ZPoint c; Double_t rx, ry, rz, yaw; Int_t kind; };

  std::vector<Puff> coaster_clouds()
  {
    TRandom3 rnd(17);
    std::vector<Puff> pv;
    const Double_t wind = 30 * TMath::DegToRad();
    const ZPoint along(std::cos(wind), std::sin(wind), 0), across(-std::sin(wind), std::cos(wind), 0);

    // Cumuli: heaps over a flat base, highest in the middle.
    for (int k = 0; k < 9; ++k)
    {
      const ZPoint c(rnd.Uniform(-140, 260), rnd.Uniform(-160, 200), rnd.Uniform(140, 170));
      const Double_t R = rnd.Uniform(14, 24);
      const int np = 7 + rnd.Integer(5);
      for (int j = 0; j < np; ++j)
      {
        const Double_t dx = rnd.Gaus(0, 0.6 * R), dy = rnd.Gaus(0, 0.6 * R);
        const Double_t r  = R * rnd.Uniform(0.45, 0.8) * (1 - 0.3 * TMath::Min(1.0, std::hypot(dx, dy) / R));
        const Double_t up = 0.8 * R * std::exp(-(dx * dx + dy * dy) / (R * R)) * rnd.Uniform(0.3, 1);
        pv.push_back({ ZPoint(c.x + dx, c.y + dy, c.z + 0.6 * r + up), r, r, 0.8 * r, 0, 0 });
      }
    }
    // Altocumuli: patches of small flat puffs in rows across the wind.
    for (int k = 0; k < 3; ++k)
    {
      const ZPoint c(rnd.Uniform(-120, 240), rnd.Uniform(-150, 190), rnd.Uniform(260, 300));
      for (int i = -3; i <= 3; ++i)
        for (int j = -3; j <= 3; ++j)
        {
          if (rnd.Uniform() < 0.25 || i*i + j*j > 12) continue;
          const ZPoint q = c + along * (18.0 * i + rnd.Uniform(-4, 4)) + across * (14.0 * j + rnd.Uniform(-4, 4));
          const Double_t r = rnd.Uniform(5, 8);
          pv.push_back({ ZPoint(q.x, q.y, q.z + rnd.Uniform(-3, 3)), r, 0.8 * r, 2.5, wind, 1 });
        }
    }
    // Cirri: faint wisps along the wind, overlapping, bending slightly.
    for (int k = 0; k < 14; ++k)
    {
      ZPoint q(rnd.Uniform(-180, 300), rnd.Uniform(-200, 240), rnd.Uniform(420, 480));
      Double_t yaw = wind + rnd.Gaus(0, 0.15);
      const int ns = 8 + rnd.Integer(8);
      for (int j = 0; j < ns; ++j)
      {
        const Double_t len = rnd.Uniform(18, 30);
        const ZPoint side = ZPoint(-std::sin(yaw), std::cos(yaw), 0) * rnd.Gaus(0, 4);
        pv.push_back({ q + side, len, rnd.Uniform(6, 12), 1, yaw + rnd.Gaus(0, 0.1), 2 });
        yaw += rnd.Gaus(0, 0.06);
        q += ZPoint(std::cos(yaw), std::sin(yaw), 0) * (0.8 * len);
      }
    }
    return pv;
  }

  // Pillars stand under every fifth point that is high and upright.
  bool coaster_pillar(const std::vector<TrackPoint>& tp, int k)
  {
    return k % 5 == 0 && tp[k].pos.z > 3 && tp[k].z.z > 0.5;
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

  // What the parts of the scene share.
  struct Coaster
  {
    Scene*                  scene;
    Double_t                m, g;      // gled units per metre, gravity in m/s^2
    ZPoint                  sun_dir;   // towards the sun
    std::vector<Puff>       clouds;
    std::vector<TrackPoint> tp;
    WSSeed*                 line;      // the centre line the cart rides
    Double_t                lap;       // seconds
    ZGlLightModel*          sky_mod;   // unlit, both faces filled
    ZNode*                  mods;      // mods used through links, not drawn in place
    ZNode                  *car, *seat;
  };

  // The sun as a lamp, low in the south-west for long shadows, far enough
  // to light like a directional one; and the sky, a sphere with a gradient
  // seen from inside, within the headset's far plane of 1000 m. The sky is
  // drawn first in the blended pass, behind everything blended.
  void coaster_sky(Coaster& C)
  {
    const Double_t m = C.m;
    CREATE_ADD_GLASS(mods, ZNode, C.scene, "SkyMods", 0);
    mods->SetRnrElements(false);
    C.mods = mods;
    const ZPoint lamp = coaster_centre + C.sun_dir * 2000.0;
    CREATE_ADD_GLASS(sun, Lamp, C.scene, "Sun", 0);
    sun->SetPos(lamp.x*m, lamp.y*m, lamp.z*m);
    sun->SetDiffuse(1, 0.95, 0.85);
    sun->SetAmbient(0.35, 0.35, 0.4);
    sun->SetLampScale(0);
    C.scene->GetGlobLamps()->Add(sun);

    CREATE_ADD_GLASS(sky_img, ZImage, C.scene, "SkyGradient", 0);
    sky_img->SetFile("sky_gradient.png");
    sky_img->Load();
    sky_img->SetEnvMode(GL_REPLACE);
    sky_img->SetMagFilter(GL_LINEAR);
    sky_img->SetMinFilter(GL_LINEAR);

    CREATE_ADD_GLASS(sky_mod, ZGlLightModel, C.scene, "SkyLightModel", 0);
    sky_mod->SetLightModelOp(ZRnrModBase::O_Off);
    sky_mod->SetFaceCullOp(ZRnrModBase::O_Off);
    sky_mod->SetShadeModelOp(ZRnrModBase::O_On);  // the pupil draws back faces as lines
    C.sky_mod = sky_mod;
    CREATE_ADD_GLASS(sky, SMorph, C.scene, "Sky", 0);
    sky->SetPos(coaster_centre.x*m, coaster_centre.y*m, 0);
    sky->RotateLF(1, 3, TMath::PiOver2());
    sky->SetScale(600*m);
    sky->SetTLevel(24); sky->SetPLevel(48);
    sky->SetTexture(sky_img);
    CREATE_ADD_GLASS(sky_blend, ZGlBlending, mods, "SkyDomeBlend", 0);
    sky_blend->SetBlendOp(ZRnrModBase::O_On);
    sky_blend->SetRnrAlgo(ZGlBlending::RA_Ordered);
    sky->MakeRnrModList(sky_mod);
    sky->MakeRnrModList(sky_blend);
  }

  // The centre line the cart rides, hidden, with its points banked; the
  // rails, spine, ties and pillars drawn from the banked frames.
  void coaster_track(Coaster& C)
  {
    const Double_t m = C.m, g = C.g;
    std::vector<TrackPoint> &tp = C.tp;
    tp = coaster_centre_line(2, g);
    const int n = tp.size();

    CREATE_ADD_GLASS(line, WSSeed, C.scene, "CentreLine", 0);
    line->SetRnrSelf(false);
    line->SetRnrElements(false);
    line->SetUpFromPoints(true);
    C.line = line;
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
    C.lap = time;
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

    int k = 0;
    for (auto it = line->begin(); it != line->end(); ++it, ++k)
    {
      WSPoint* p = dynamic_cast<WSPoint*>(it->fLens);
      if (p) p->SetTrans(frame(tp[k % n].pos * m, tp[k % n].x, tp[k % n].z));
    }

    const ZColor steel(0.75, 0.75, 0.8), red(0.8, 0.15, 0.1);
    WSSeed* rail_l = rail_seed(C.scene, "RailLeft",  0.06*m, steel);
    WSSeed* rail_r = rail_seed(C.scene, "RailRight", 0.06*m, steel);
    WSSeed* spine  = rail_seed(C.scene, "Spine",     0.18*m, red);
    for (int k = 0; k <= n; ++k)
    {
      const TrackPoint& p = tp[k % n];
      const ZPoint y = p.z.Cross(p.x);
      add_point(rail_l, frame((p.pos + y * 0.6) * m, p.x, p.z), 0.06*m, 1);
      add_point(rail_r, frame((p.pos - y * 0.6) * m, p.x, p.z), 0.06*m, 1);
      add_point(spine,  frame((p.pos - p.z * 0.35) * m, p.x, p.z), 0.18*m, 1);
    }

    CREATE_ADD_GLASS(ties, ZNode, C.scene, "Ties", 0);
    CREATE_ADD_GLASS(pillars, ZNode, C.scene, "Pillars", 0);
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
      if (coaster_pillar(tp, k))
      {
        const Double_t h = p.pos.z - 0.5;
        CREATE_ADD_GLASS(pil, Box, pillars, GForm("Pillar %d", k), 0);
        pil->SetPos(p.pos.x*m, p.pos.y*m, 0.5*h*m);
        pil->SetABC(0.4*m, 0.4*m, h*m);
        pil->SetColor(0.85, 0.85, 0.8);
      }
    }
  }

  // The ground, 400 m square in cells of 2 m, coloured by height, with a
  // light map of the hills' own shadows and those of the track, pillars and
  // cumuli; and the crater lakes.
  void coaster_ground(Coaster& C)
  {
    const Double_t m = C.m;
    const ZPoint& sun_dir = C.sun_dir;
    const std::vector<TrackPoint>& tp = C.tp;
    const int n = tp.size();
    HeightGrid hg;
    hg.nc = 201; hg.dc = 2; hg.x0 = coaster_centre.x - 200; hg.y0 = coaster_centre.y - 200;
    const int nc = hg.nc;
    hg.h.resize(nc * nc);
    for (int iy = 0; iy < nc; ++iy)
      for (int ix = 0; ix < nc; ++ix)
      {
        const Double_t x = hg.x0 + ix * hg.dc, y = hg.y0 + iy * hg.dc;
        Double_t d2 = 1e12;
        for (const TrackPoint& p : tp)
          d2 = TMath::Min(d2, (p.pos.x - x) * (p.pos.x - x) + (p.pos.y - y) * (p.pos.y - y));
        hg.h[iy * nc + ix] = coaster_ground(x, y, std::sqrt(d2));
      }
    Double_t h_min = 1e9, h_max = -1e9;
    std::vector<Float_t> hf(nc * nc);
    for (int i = 0; i < nc * nc; ++i)
    {
      hf[i] = hg.h[i] * m;
      h_min = TMath::Min(h_min, hg.h[i]); h_max = TMath::Max(h_max, hg.h[i]);
    }

    // The palette in metres, mapped onto the terrain's range.
    struct Band { Double_t h, r, g, b; };
    const Band bands[] = {
      { -6, 0.00, 0.35, 0.55 }, { -5, 0.10, 0.75, 0.80 }, { -4.5, 0.93, 0.86, 0.60 },
      { -0.5, 0.90, 0.82, 0.55 }, { 0.3, 0.35, 0.75, 0.20 }, { 12, 0.10, 0.50, 0.15 },
      { 30, 0.08, 0.35, 0.12 }, { 45, 0.40, 0.30, 0.20 }, { 60, 0.55, 0.50, 0.45 },
      { 75, 0.92, 0.92, 0.95 }
    };
    const TString pov = GForm("%s/coaster_palette_%d.pov", gSystem->TempDirectory(), gSystem->GetPid());
    FILE* fp = fopen(pov.Data(), "w");
    fprintf(fp, "color_map {\n");
    for (const Band& b : bands)
      fprintf(fp, "  [%f color rgbt <%f, %f, %f, 0>]\n",
              TMath::Max(0.0, TMath::Min(1.0, (b.h - h_min) / (h_max - h_min))), b.r, b.g, b.b);
    fprintf(fp, "}\n");
    fclose(fp);
    CREATE_ADD_GLASS(palette, RGBAPalette, C.scene, "Tropical", 0);
    palette->SetMarksFromPOVFile(pov.Data());
    gSystem->Unlink(pov.Data());

    // The light map, 1024 texels over 400 m. The hills: a march towards the
    // sun from each cell. The track and pillars: their points projected
    // along the sun onto the ground.
    const int    nl = 1024;
    const Double_t texel = 400.0 / nl, shade = 0.55;
    std::vector<Double_t> lit(nc * nc, 1);
    for (int iy = 0; iy < nc; ++iy)
      for (int ix = 0; ix < nc; ++ix)
      {
        ZPoint q(hg.x0 + ix * hg.dc, hg.y0 + iy * hg.dc, hg.h[iy * nc + ix] + 0.05);
        for (int k = 0; k < 160 && lit[iy * nc + ix] == 1; ++k)
        {
          q += sun_dir * 2.0;
          if (hg.at(q.x, q.y) > q.z) lit[iy * nc + ix] = shade;
        }
      }
    std::vector<UChar_t> lm(nl * nl);
    auto texel_of = [&](Double_t x, Double_t y, int& a, int& r) {
      a = int((x - hg.x0) / texel); r = int((1 - (y - hg.y0) / 400) * nl); };
    for (int r = 0; r < nl; ++r)
      for (int a = 0; a < nl; ++a)
      {
        const Double_t fx = (a + 0.5) / nl * (nc - 1), fy = (1 - (r + 0.5) / nl) * (nc - 1);
        const int ix = TMath::Min(nc - 2, int(fx)), iy = TMath::Min(nc - 2, int(fy));
        const Double_t ax = fx - ix, ay = fy - iy;
        const Double_t v = (1 - ay) * ((1 - ax) * lit[iy * nc + ix] + ax * lit[iy * nc + ix + 1])
                         + ay * ((1 - ax) * lit[(iy + 1) * nc + ix] + ax * lit[(iy + 1) * nc + ix + 1]);
        lm[r * nl + a] = UChar_t(255 * v);
      }
    auto stamp = [&](const ZPoint& p, Double_t radius, Double_t dark) {
      Double_t t = 0;
      for (int k = 0; k < 4; ++k) t = (p.z - hg.at(p.x - t * sun_dir.x, p.y - t * sun_dir.y)) / sun_dir.z;
      int a0, r0;
      texel_of(p.x - t * sun_dir.x, p.y - t * sun_dir.y, a0, r0);
      const int rt = int(radius / texel) + 1;
      for (int r = r0 - rt; r <= r0 + rt; ++r)
        for (int a = a0 - rt; a <= a0 + rt; ++a)
          if (a >= 0 && a < nl && r >= 0 && r < nl && (a - a0) * (a - a0) + (r - r0) * (r - r0) <= rt * rt)
            lm[r * nl + a] = TMath::Min(lm[r * nl + a], UChar_t(255 * dark));
    };
    for (int k = 0; k < n; ++k)
    {
      const ZPoint &p = tp[k].pos, &q = tp[(k + 1) % n].pos;
      for (int j = 0; j < 7; ++j) stamp(p + (q - p) * (j / 7.0), 0.75, shade);
      if (coaster_pillar(tp, k))
        for (Double_t z = 0.5; z < p.z; z += 0.3) stamp(ZPoint(p.x, p.y, z), 0.2, shade);
    }
    for (const Puff& c : C.clouds)
      if (c.kind == 0) stamp(c.c, 0.9 * c.rx, 0.72);
    const TString pgm = GForm("%s/coaster_light_%d.pgm", gSystem->TempDirectory(), gSystem->GetPid());
    fp = fopen(pgm.Data(), "wb");
    fprintf(fp, "P5\n%d %d\n255\n", nl, nl);
    fwrite(&lm[0], 1, nl * nl, fp);
    fclose(fp);
    CREATE_ADD_GLASS(light_map, ZImage, C.scene, "LightMap", 0);
    light_map->SetFile(pgm.Data());
    light_map->Load();
    light_map->SetEnvMode(GL_MODULATE);
    light_map->SetMagFilter(GL_LINEAR);
    light_map->SetMinFilter(GL_LINEAR);
    gSystem->Unlink(pgm.Data());

    CREATE_ADD_GLASS(ground, RectTerrain, C.scene, "Ground", 0);
    ground->SetFromArray(nc, nc, &hf[0]);
    ground->SetDx(hg.dc*m); ground->SetDy(hg.dc*m);
    ground->SetPos(hg.x0*m, hg.y0*m, 0);
    ground->SetRibbon(palette);
    ground->SetTexture(light_map);
    ground->SetRnrMode(RectTerrain::RM_SmoothTring);

    // The crater lakes, flat discs just below their rims.
    for (const Crater& c : coaster_craters)
    {
      const Double_t level = coaster_lake(c);
      if (level < -1e8) continue;
      CREATE_ADD_GLASS(lake, SMorph, C.scene, "Lake", 0);
      lake->SetPos(c.x*m, c.y*m, level*m);
      lake->SetScales(1.05*c.r*m, 1.05*c.r*m, 0.02*m);
      lake->SetTLevel(6); lake->SetPLevel(48);
      lake->SetColor(0.1, 0.62, 0.75);
    }
  }

  // The cart, an open half shell, textured and lit from both sides, and the
  // seat the camera rides.
  void coaster_cart(Coaster& C)
  {
    const Double_t m = C.m;
    CREATE_ADD_GLASS(car, ZNode, C.scene, "Car", 0);
    car->SetTrans(frame(C.tp[0].pos * m, C.tp[0].x, C.tp[0].z));
    C.car = car;
    CREATE_ADD_GLASS(cart_img, ZImage, C.scene, "CartTexture", 0);
    cart_img->SetFile("checker_4.png");
    cart_img->Load();
    CREATE_ADD_GLASS(cart_mod, ZGlLightModel, C.scene, "CartLightModel", 0);
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
    C.seat = seat;
  }

  // The sun's disc where the light comes from, with a glow, and the clouds.
  // The disc and the glow stand for the far sun, so they are drawn in order
  // after the sky; the clouds follow, back to front. Light models turn depth
  // writes back on: the cumuli keep them; the thin clouds and the sun do not.
  void coaster_sun_and_clouds(Coaster& C)
  {
    const Double_t m = C.m;
    CREATE_ADD_GLASS(blend, ZGlBlending, C.mods, "SkyBlending", 0);
    blend->SetBlendOp(ZRnrModBase::O_On);
    blend->SetRnrAlgo(ZGlBlending::RA_Sorted);
    blend->SetDepthMaskOp(ZRnrModBase::O_Off);
    CREATE_ADD_GLASS(cloud_mod, ZGlLightModel, C.mods, "CloudLightModel", 0);
    cloud_mod->SetLightModelOp(ZRnrModBase::O_On);
    cloud_mod->SetLiMoTwoSide(true);
    cloud_mod->SetShadeModelOp(ZRnrModBase::O_On);
    cloud_mod->SetFaceCullOp(ZRnrModBase::O_Off);
    CREATE_ADD_GLASS(cumulus_blend, ZGlBlending, C.mods, "CumulusBlend", 0);
    cumulus_blend->SetBlendOp(ZRnrModBase::O_On);
    cumulus_blend->SetRnrAlgo(ZGlBlending::RA_Sorted);
    CREATE_ADD_GLASS(cumulus_mod, ZGlLightModel, C.mods, "CumulusLightModel", 0);
    cumulus_mod->SetLightModelOp(ZRnrModBase::O_On);
    cumulus_mod->SetLiMoAmbient(0.55, 0.55, 0.6);
    cumulus_mod->SetLiMoTwoSide(true);
    cumulus_mod->SetShadeModelOp(ZRnrModBase::O_On);
    cumulus_mod->SetFaceCullOp(ZRnrModBase::O_Off);
    const ZPoint sun_at = coaster_centre + C.sun_dir * 560.0;
    CREATE_ADD_GLASS(sun_disc, SMorph, C.scene, "SunDisc", 0);
    sun_disc->SetPos(sun_at.x*m, sun_at.y*m, sun_at.z*m);
    sun_disc->SetScale(18*m);
    sun_disc->SetTLevel(16); sun_disc->SetPLevel(24);
    sun_disc->SetColor(1, 0.97, 0.88);
    CREATE_ADD_GLASS(sun_blend, ZGlBlending, C.mods, "SunBlend", 0);
    sun_blend->SetBlendOp(ZRnrModBase::O_On);
    sun_blend->SetRnrAlgo(ZGlBlending::RA_Ordered);
    sun_blend->SetDepthMaskOp(ZRnrModBase::O_Off);
    sun_disc->MakeRnrModList(C.sky_mod);
    sun_disc->MakeRnrModList(sun_blend);
    CREATE_ADD_GLASS(glow, ZGlBlending, C.mods, "SunGlow", 0);
    glow->SetBlendOp(ZRnrModBase::O_On);
    glow->SetRnrAlgo(ZGlBlending::RA_Ordered);
    glow->SetBDstFac(GL_ONE);
    glow->SetDepthMaskOp(ZRnrModBase::O_Off);
    // Faint additive layers, so the glow fades outwards instead of saturating.
    for (Double_t r : { 30.0, 50.0, 80.0 })
    {
      CREATE_ADD_GLASS(halo, SMorph, C.scene, "SunHalo", 0);
      halo->SetPos(sun_at.x*m, sun_at.y*m, sun_at.z*m);
      halo->SetScale(r*m);
      halo->SetTLevel(16); halo->SetPLevel(24);
      halo->SetColor(1, 0.9, 0.7, 0.07);
      halo->MakeRnrModList(C.sky_mod);
      halo->MakeRnrModList(glow);
    }

    CREATE_ADD_GLASS(cumuli, ZNode, C.scene, "Cumuli", 0);
    cumuli->MakeRnrModList(cumulus_mod);
    cumuli->MakeRnrModList(cumulus_blend);
    CREATE_ADD_GLASS(altocumuli, ZNode, C.scene, "Altocumuli", 0);
    altocumuli->MakeRnrModList(cloud_mod);
    altocumuli->MakeRnrModList(blend);
    CREATE_ADD_GLASS(cirri, ZNode, C.scene, "Cirri", 0);
    cirri->MakeRnrModList(C.sky_mod);
    cirri->MakeRnrModList(blend);
    ZNode* cloud_dirs[3] = { cumuli, altocumuli, cirri };
    const ZColor cloud_cols[3] = { ZColor(1, 1, 1), ZColor(0.97, 0.97, 1, 0.85), ZColor(1, 1, 1, 0.16) };
    for (const Puff& c : C.clouds)
    {
      CREATE_ADD_GLASS(puff, SMorph, cloud_dirs[c.kind], "Puff", 0);
      puff->SetPos(c.c.x*m, c.c.y*m, c.c.z*m);
      puff->RotateLF(1, 2, c.yaw);
      puff->SetScales(c.rx*m, c.ry*m, c.rz*m);
      puff->SetTLevel(c.kind == 2 ? 6 : 8); puff->SetPLevel(c.kind == 2 ? 8 : 12);
      const ZColor& col = cloud_cols[c.kind];
      puff->SetColor(col.r(), col.g(), col.b(), col.a());
    }
  }
}

void coaster(Double_t mpu=0.2, Bool_t flight=false)
{
  ASSERT_MACRO(sun_demos);
  Gled::theOne->AssertLibSet("Geom1");

  Coaster C;
  C.m = 1 / mpu;
  C.g = 9.81;
  const Double_t sun_el = 35 * TMath::DegToRad(), sun_az = 220 * TMath::DegToRad();
  C.sun_dir.Set(std::cos(sun_el) * std::cos(sun_az), std::cos(sun_el) * std::sin(sun_az), std::sin(sun_el));
  C.clouds = coaster_clouds();

  C.scene = new Scene("Coaster Scene");
  g_queen->CheckIn(C.scene);
  g_queen->Add(C.scene);
  g_scene = C.scene;

  coaster_sky(C);
  coaster_sun_and_clouds(C);
  coaster_track(C);
  coaster_ground(C);
  coaster_cart(C);

  CREATE_ADD_GLASS(etor, Eventor, C.scene, "Eventor", 0);
  etor->SetInterBeatMS(gSystem->Getenv("GLED_XR") ? 11 : 20);
  CREATE_ADD_GLASS(tmaker, TimeMaker, etor, "TimeMaker", 0);
  CREATE_ATT_GLASS(walker, WSWalker, tmaker, AddClient, "Walker", 0);
  walker->SetSeed(C.line);
  walker->SetNode(C.car);
  walker->SetLoop(true);
  walker->SetFlight(flight);
  walker->SetGravity(C.g * C.m);

  printf("coaster: %d points, %.0f m, a lap in %.0f s.\n", int(C.tp.size()), 2.0 * C.tp.size(), C.lap);

  if (Gled::theOne->HasGUILibs())
  {
    eye();
    g_pupil->SetCameraBase(C.seat);
    g_pupil->SetXrTilt(PupilInfo::XT_Ride);
    g_pupil->SetNearClip(0.1 * C.m);
    g_pupil->SetFarClip(1500 * C.m);
    g_pupil->SetClearColor(0.6, 0.72, 0.9);
    // The camera sits on the seat once the pupil exists.
    TTimer::SingleShot(3000, "gled::PupilInfo", g_pupil, "EmitCameraHomeRay()");
  }

  etor->Start();
}
