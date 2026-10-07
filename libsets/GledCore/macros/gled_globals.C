// Common global variables.

#ifndef GLED_GLED_GLOBALS_C
#define GLED_GLED_GLOBALS_C

#define ASSERT_MACRO(_mac_, ...) if (! gled_ ## _mac_ ## _called) _mac_(__VA_ARGS__);

gled::Gled*       g_gled       = 0;

// Saturn related information.

gled::Saturn*     g_saturn     = 0;

gled::ZKing*      g_sun_king   = 0;
gled::ZKing*      g_king       = 0;
gled::ZFireKing*  g_fire_king  = 0;

gled::ZSunQueen*  g_sun_queen  = 0;
gled::ZFireQueen* g_fire_queen = 0;

// Current containers (used by eye.C (a GUI spawner) and demo scripts.
// Can be modified by the user.

gled::ZQueen*     g_queen       = 0;
gled::Scene*      g_scene       = 0;

bool gled_gled_globals_called = false;

void gled_globals()
{
  gled_gled_globals_called = true;

  using namespace gled;

  g_gled = Gled::theOne;

  g_saturn = Gled::theOne->GetSaturn();
  if (g_saturn == 0)
  {
    throw Exc_t("gled_globals: Sun is not spawned.");
  }

  g_sun_king   = g_saturn->GetSunKing();
  g_king       = g_saturn->GetKing();
  g_fire_king  = g_saturn->GetFireKing();

  g_sun_queen  = g_saturn->GetSunQueen();
  g_fire_queen = g_saturn->GetFireQueen();
}

#endif
