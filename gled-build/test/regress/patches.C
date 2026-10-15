// Builds terrain patches on GTS spheres and checks the patch table of
// TringTvor; see test/README.md.
#include <Gled/GledNS.h>

#include "sun_demos.C"

#include <cmath>
#include <set>

using namespace gled;

namespace
{
  int n_fail = 0;

  void check(bool ok, const char* what, int i = -1)
  {
    if (ok) return;
    if (++n_fail <= 20) printf("patches: FAIL %s (%d)\n", what, i);
  }

  // Every triangle in the vertex and box ranges, every normal in the cone.
  template <typename P>
  void check_bounds(TringTvor& tt, const P& p, int i)
  {
    const float eps = 1e-5f * (1 + tt.BoundingBoxHalfDiagonal());
    for (int t = p.fFirstT; t < p.fFirstT + p.fNT; ++t)
    {
      const int* T = tt.Triangle(t);
      for (int j = 0; j < 3; ++j)
      {
        check(T[j] >= p.fMinV && T[j] <= p.fMaxV, "vertex range", i);
        const float* V = tt.Vertex(T[j]);
        for (int k = 0; k < 3; ++k)
          check(std::fabs(V[k] - p.fCtrExt[k]) <= p.fCtrExt[k + 3] + eps, "box", i);
      }
      float n[3];
      if (tt.CalculateTriangleNormal(t, n) > 0 && p.fConeSin < 1)
      {
        const double c = p.fConeAxis[0]*n[0] + p.fConeAxis[1]*n[1] + p.fConeAxis[2]*n[2];
        check(c >= std::sqrt(1 - double(p.fConeSin) * p.fConeSin) - 1e-6, "cone", i);
      }
    }
  }

  // The triangles as vertex coordinates, which a reorder must keep.
  std::multiset<std::vector<float>> triangles(TringTvor& tt)
  {
    std::multiset<std::vector<float>> r;
    for (int t = 0; t < tt.mNTrings; ++t)
    {
      std::vector<float> a;
      for (int j = 0; j < 3; ++j)
        for (int k = 0; k < 3; ++k) a.push_back(tt.Vertex(tt.Triangle(t)[j])[k]);
      r.insert(a);
    }
    return r;
  }

  TriMesh* make_mesh(const char* name, int order, int faces_per_patch, unsigned& stamp)
  {
    GTSurf* gs = new GTSurf(name);
    g_queen->CheckIn(gs);
    gs->GenerateSphere(order);
    TriMesh* tm = new TriMesh(name);
    g_queen->CheckIn(tm);
    tm->ImportGTSurf(gs);
    TringTvor& tt = *tm->GetTTvor();
    const std::multiset<std::vector<float>> before = triangles(tt);

    tm->BuildPatches(faces_per_patch);
    check(tt.mPatchStamp != 0 && tt.mPatchStamp != stamp, "stamp");
    stamp = tt.mPatchStamp;
    check(triangles(tt) == before, "triangles kept");

    // The patches tile the triangles; the vertices come in first-use order.
    int next = 0, seen = -1;
    for (int i = 0; i < (int) tt.mPatches.size(); ++i)
    {
      const TringTvor::Patch& p = tt.mPatches[i];
      check(p.fFirstT == next, "patch order", i);
      next += p.fNT;
      check_bounds(tt, p, i);
    }
    check(next == tt.mNTrings, "patches cover the triangles");
    for (int i = 0; i < 3 * tt.mNTrings; ++i)
    {
      check(tt.mTrings[i] <= seen + 1, "first-use order", i);
      seen = std::max(seen, tt.mTrings[i]);
    }

    // Each node of the tree is its two children.
    for (int i = 0; i < (int) tt.mPatchNodes.size(); ++i)
    {
      const TringTvor::PatchNode& n = tt.mPatchNodes[i];
      check_bounds(tt, n, i);
      if (n.fChild[0] < 0) continue;
      const TringTvor::PatchNode &a = tt.mPatchNodes[n.fChild[0]], &b = tt.mPatchNodes[n.fChild[1]];
      check(a.fFirstT == n.fFirstT && a.fFirstT + a.fNT == b.fFirstT &&
            b.fFirstT + b.fNT == n.fFirstT + n.fNT, "node children", i);
    }

    // Strips stay in their patches and cover each triangle once, also when
    // generated again over existing ones.
    for (int pass = 0; pass < 2; ++pass)
    {
      tt.GenerateTriangleStrips();
      std::vector<int> cover(tt.mNTrings, 0);
      for (const TringTvor::Patch& p : tt.mPatches)
        for (int s = p.fFirstStrip; s < p.fFirstStrip + p.fNStrips; ++s)
        {
          const int beg = tt.mStripBegs[s] - tt.mStripEls;
          for (int i = 2; i < tt.mStripLens[s]; ++i)
          {
            const int t = tt.mStripTrings[beg + i];
            check(t >= p.fFirstT && t < p.fFirstT + p.fNT, "strip in patch", t);
            ++cover[t];
          }
        }
      for (int t = 0; t < tt.mNTrings; ++t) check(cover[t] == 1, "strip cover", t);
    }

    double sin_sum = 0;
    for (const TringTvor::Patch& p : tt.mPatches) sin_sum += p.fConeSin;
    printf("patches: %s, %d triangles, %d patches, %d nodes, mean cone sine %.2f\n", name,
           tt.mNTrings, (int) tt.mPatches.size(), (int) tt.mPatchNodes.size(),
           sin_sum / tt.mPatches.size());
    return tm;
  }
}

void patches()
{
  ASSERT_MACRO(sun_demos);
  Gled::theOne->AssertLibSet("Var1");

  unsigned stamp = 0;
  make_mesh("Sphere 5 by 300", 5, 300, stamp);
  make_mesh("Sphere 7 by 1000", 7, 1000, stamp);
  TriMesh* small = make_mesh("Sphere 3 by 1000", 3, 1000, stamp);

  // Adding triangles wipes the patches.
  TringTvor& tt = *small->GetTTvor();
  tt.AddTriangles(0);
  check( ! tt.HasPatches() && tt.mPatchStamp != stamp, "wipe on AddTriangles");

  printf("patches: %s, %d failures\n", n_fail ? "FAILED" : "all passed", n_fail);
}
