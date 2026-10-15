// A ZNodeLink to its own ancestor; see test/README.md.
#include <Gled/GledNS.h>

#include "sun_demos.C"
#include "eye.C"

using namespace gled;

void rnr_cycle()
{
  ASSERT_MACRO(sun_demos);

  Scene* sc = new Scene("RnrCycle");
  g_queen->CheckIn(sc);
  g_queen->Add(sc);
  g_scene = sc;

  Sphere* top = new Sphere("Top");
  top->SetRadius(0.5);
  g_queen->CheckIn(top);
  sc->Add(top);

  Sphere* child = new Sphere("Child");
  child->SetRadius(0.25);
  child->MoveLF(1, 1);
  g_queen->CheckIn(child);
  top->Add(child);

  ZNodeLink* back = new ZNodeLink("Back");
  back->MoveLF(2, 1);
  g_queen->CheckIn(back);
  child->Add(back);
  back->SetLens(top);

  eye();
}
