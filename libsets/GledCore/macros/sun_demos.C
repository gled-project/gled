#ifndef GLED_SUN_DEMOS_C
#define GLED_SUN_DEMOS_C

// Generick initialization of top level script variables for Sun.
// Also creates global queen ZQueen("Scenes"), then used by demo-scripts
// ta add in scene objects.
// Can be called from other scripts, as a command-line option or from
// TRint prompt.

#include "sun.C"

bool gled_sun_demos_called = false;

void sun_demos(Int_t queen_id_size=256*1024)
{
  gled_sun_demos_called = true;

  using namespace gled;

  ASSERT_MACRO(sun);

  g_queen = new ZQueen(queen_id_size, "Scenes", "Goddess of Ver");
  g_sun_king->Enthrone(g_queen);
  g_queen->SetMandatory(true);
}

#endif
