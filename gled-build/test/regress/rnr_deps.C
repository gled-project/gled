// Changes the structure under two ZNodeLinks after the pupil is up; see test/README.md.
#include <Gled/GledNS.h>

#include "sun_demos.C"
#include "eye.C"

using namespace gled;

void at(Double_t sec, ZMIR* mir)
{
  GTime t = GTime::Now();
  t += sec;
  g_saturn->DelayedShootMIR(mir, t);
}

void rnr_deps()
{
  ASSERT_MACRO(sun_demos);

  Scene* sc = new Scene("RnrDeps");
  g_queen->CheckIn(sc);
  g_queen->Add(sc);
  g_scene = sc;

  // Linked twice, not added to the scene.
  Text* target = new Text("Target");
  target->SetText("Target");
  g_queen->CheckIn(target);

  ZNodeLink* link1 = 0;
  for (int i = 0; i < 2; ++i)
  {
    ZNodeLink* l = new ZNodeLink(GForm("Link%d", i + 1));
    l->MoveLF(2, 3*i);
    g_queen->CheckIn(l);
    sc->Add(l);
    l->SetLens(target);
    if (i == 0) link1 = l;
  }

  ZRlFont* font = new ZRlFont("Font");
  g_queen->CheckIn(font);

  Sphere* child = new Sphere("Child");
  child->SetRadius(0.5);
  g_queen->CheckIn(child);

  // The target's links are rendered inline by each link.
  at(4,  target->S_SetFont(font));
  // The target's elements are rendered through each link's list bits.
  at(6,  target->S_Add(child));
  at(8,  g_queen->S_RemoveLens(child));
  at(10, link1->S_SetLens(0));

  eye();
}
