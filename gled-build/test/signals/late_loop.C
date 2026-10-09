// SIGTERM while a macro pumps events after Run(); gled must exit after "pump end".
//   sigtest.sh out.txt 'pump start' late_loop.C

#include <Gled/GledNS.h>
using namespace gled;

TTimer *g_pump_timer = 0;

void pump()
{
  g_pump_timer->TurnOff();
  printf("pump start\n");
  for (int i = 0; i < 100; ++i)
  {
    gSystem->ProcessEvents();
    gSystem->Sleep(100);
  }
  printf("pump end\n");
}

void late_loop()
{
  g_pump_timer = new TTimer("pump()", 3000, kTRUE);
  g_pump_timer->TurnOn();
}
