// Generick initialization of top level script variables for Sun.
// Can be called from other scripts, as a command-line option or from
// TRint prompt.

#ifndef GLED_SUN_C
#define GLED_SUN_C

#include "gled_globals.C"

bool gled_sun_called = false;

void sun()
{
  gled_sun_called = true;

  using namespace gled;

  if (Gled::theOne->GetSaturn() == 0)
  {
    Gled::theOne->SpawnSun();
    if (Gled::theOne->GetSaturn()->GetSaturnInfo()->GetUseAuth())
    {
      Gled::Macro("std_auth.C");
    }
  }

  ASSERT_MACRO(gled_globals);
}

#endif
