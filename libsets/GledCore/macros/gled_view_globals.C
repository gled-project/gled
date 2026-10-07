#ifndef GLED_GLED_VIEW_GLOBALS_C
#define GLED_GLED_VIEW_GLOBALS_C

// Common global variables related to GUI and viewers.

#include "Gled/GledNS.h"

gled::EyeInfo*       g_eye   = 0;
gled::ShellInfo*     g_shell = 0;
gled::NestInfo*      g_nest  = 0;
gled::PupilInfo*     g_pupil = 0;

bool gled_gled_view_globals_called = false;

void gled_view_globals()
{
  gled_gled_view_globals_called = true;

  using namespace gled;

  TString pupil_lib("libGledCore_Pupils.so");
  GledNS::LoadSo(pupil_lib);
}

#endif
