// Two overlapping translucent spheres, the near one first in the tree, under
// each ZGlBlending::RnrAlgo; checks the colour of the overlap in dumped images.
// With GLED_RNR_BLENDED=0 it expects the colours of the pass off; see test/README.md.
#include <Gled/GledNS.h>

#include "sun_demos.C"
#include "eye.C"

#include <cmath>
#include <cstdio>
#include <vector>

using namespace gled;

namespace
{
  Sphere      *bl_near = 0, *bl_far = 0;
  ZGlBlending *bl_blend = 0;
  TString      bl_stem;
  const char  *bl_shots[] = { "near", "far", "sorted", "ordered", "inplace" };

  TString bl_file(int i) { return GForm("%s_%s", bl_stem.Data(), bl_shots[i]); }

  // Pupil::dump_image writes an 18-byte header and BGR rows.
  bool bl_read(int i, std::vector<UChar_t>& pix)
  {
    FILE *f = fopen(bl_file(i) + ".tga", "rb");
    if ( ! f) return false;
    fseek(f, 0, SEEK_END);
    long n = ftell(f) - 18;
    fseek(f, 18, SEEK_SET);
    pix.resize(n > 0 ? n : 0);
    bool ok = n > 0 && (long) fread(&pix[0], 1, n, f) == n;
    fclose(f);
    gSystem->Unlink(bl_file(i) + ".tga");
    return ok;
  }

  int bl_failed = 0;

  void bl_check(const char* what, bool ok, const char* detail)
  {
    printf("blended: %-40s %s %s\n", what, ok ? "ok" : "FAILED", detail);
    if ( ! ok) ++bl_failed;
  }

  void bl_analyse()
  {
    std::vector<UChar_t> img[5];
    for (int i = 0; i < 5; ++i)
    {
      if ( ! bl_read(i, img[i]) || img[i].size() != img[0].size())
      {
        bl_check(GForm("image %s", bl_shots[i]), false, "missing or of another size");
        printf("blended: FAILED\n");
        return;
      }
    }
    // The overlap: red in the near-only image and blue in the far-only one.
    std::vector<size_t> ov;
    for (size_t p = 0; p + 2 < img[0].size(); p += 3)
      if (img[0][p + 2] > 64 && img[1][p] > 64) ov.push_back(p);
    bl_check("overlap found", ov.size() > 500, GForm("(%d pixels)", (int) ov.size()));
    if (ov.size() <= 500) { printf("blended: FAILED\n"); return; }

    const char *off = gSystem->Getenv("GLED_RNR_BLENDED");
    const bool  pass_on = ! (off && off[0] == '0');
    // Alpha 0.5 on black: near over far, far over near, and the near one's depth hiding the far one.
    const int exp_on[3][3]  = { { 128, 0, 64 }, { 64, 0, 128 }, { 128, 0, 0 } };
    const int exp_off[3][3] = { { 128, 0, 0 },  { 128, 0, 0 },  { 128, 0, 0 } };
    const int tol = 4;
    for (int m = 0; m < 3; ++m)
    {
      const int *e = pass_on ? exp_on[m] : exp_off[m];
      const std::vector<UChar_t> &im = img[2 + m];
      int    n_off = 0;
      double sum[3] = { 0, 0, 0 };
      for (size_t p : ov)
      {
        const int c[3] = { im[p + 2], im[p + 1], im[p] };
        for (int k = 0; k < 3; ++k) sum[k] += c[k];
        if (std::abs(c[0] - e[0]) > tol || std::abs(c[1] - e[1]) > tol || std::abs(c[2] - e[2]) > tol) ++n_off;
      }
      const double nn = ov.size();
      bl_check(GForm("%s, pass %s", bl_shots[2 + m], pass_on ? "on" : "off"), n_off <= 0.01 * nn,
               GForm("(mean %.0f,%.0f,%.0f, expected %d,%d,%d, %d of %d pixels off)",
                     sum[0] / nn, sum[1] / nn, sum[2] / nn, e[0], e[1], e[2], n_off, (int) nn));
    }
    printf("blended: %s\n", bl_failed ? "FAILED" : "all passed");
  }
}

void blended_tick()
{
  static int k = 0;
  switch (k++)
  {
    case 0:  g_pupil->SetCameraBase((ZNode*) g_scene->FindLensByPath("Cam")); g_pupil->EmitCameraHomeRay(); break;
    case 1:  bl_far->SetRnrSelf(false); break;
    case 2:  g_pupil->EmitDumpImageRay(bl_file(0), 1, false, false); break;
    case 3:  bl_far->SetRnrSelf(true); bl_near->SetRnrSelf(false); break;
    case 4:  g_pupil->EmitDumpImageRay(bl_file(1), 1, false, false); break;
    case 5:  bl_near->SetRnrSelf(true); bl_blend->SetRnrAlgo(ZGlBlending::RA_Sorted); break;
    case 6:  g_pupil->EmitDumpImageRay(bl_file(2), 1, false, false); break;
    case 7:  bl_blend->SetRnrAlgo(ZGlBlending::RA_Ordered); break;
    case 8:  g_pupil->EmitDumpImageRay(bl_file(3), 1, false, false); break;
    case 9:  bl_blend->SetRnrAlgo(ZGlBlending::RA_InPlace); break;
    case 10: g_pupil->EmitDumpImageRay(bl_file(4), 1, false, false); break;
    case 12: bl_analyse(); break;
  }
}

void blended()
{
  ASSERT_MACRO(sun_demos);

  Scene* sc = new Scene("Blended"); g_queen->CheckIn(sc); g_queen->Add(sc); g_scene = sc;
  ZGlLightModel* lm = new ZGlLightModel("Unlit"); g_queen->CheckIn(lm); g_queen->Add(lm);
  lm->SetLightModelOp(ZRnrModBase::O_Off);
  lm->SetFaceCullOp(ZRnrModBase::O_On);
  bl_blend = new ZGlBlending("Blend"); g_queen->CheckIn(bl_blend); g_queen->Add(bl_blend);
  bl_blend->SetBlendOp(ZRnrModBase::O_On);

  ZNode* bl_pair = new ZNode("Pair"); g_queen->CheckIn(bl_pair); sc->Add(bl_pair);
  bl_pair->MakeRnrModList(lm); bl_pair->MakeRnrModList(bl_blend);
  bl_near = new Sphere("Near"); g_queen->CheckIn(bl_near); bl_pair->Add(bl_near);
  bl_near->SetPos(10, 0, 0); bl_near->SetRadius(1.5); bl_near->SetLOD(40); bl_near->SetColor(1, 0, 0, 0.5);
  bl_far = new Sphere("Far"); g_queen->CheckIn(bl_far); bl_pair->Add(bl_far);
  bl_far->SetPos(20, 2.5, 0); bl_far->SetRadius(3); bl_far->SetLOD(40); bl_far->SetColor(0, 0, 1, 0.5);
  ZNode* cam = new ZNode("Cam"); g_queen->CheckIn(cam); sc->Add(cam);

  bl_stem = GForm("%s/blended_%d", gSystem->TempDirectory(), gSystem->GetPid());

  eye();
  g_pupil->SetClearColor(0, 0, 0);
  TTimer* t = new TTimer("blended_tick()", 1000, kTRUE);
  t->TurnOn();
}
