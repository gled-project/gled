// Starts a Saturn connected to the master given with --master, and an Eye
// with a pupil on the first scene of the queen "Scenes". Run: gled -m <master> moon.C

#include "gled_globals.C"
#include "eye.C"

using namespace gled;

void moon()
{
  if (Gled::theOne->GetSaturn() == 0)
  {
    Gled::theOne->SpawnSaturn();
  }

  ASSERT_MACRO(gled_globals);

  g_queen = (ZQueen*) g_sun_king->GetElementByName("Scenes");
  if (g_queen)
    g_scene = dynamic_cast<Scene*>(g_queen->FrontElement());

  eye();
  setup_pupil_up_reference();
}
