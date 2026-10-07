// SIGTERM during a macro that runs after TApplication::Run() has started.
// A timer runs pump() 3 s after start-up; it calls ProcessEvents() for
// 10 s.  Send SIGTERM while "pump start" is the last line: gled should
// exit right after "pump end", not before and not never.
//
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
