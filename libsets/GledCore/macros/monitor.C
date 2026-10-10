// Puts a GledMonitor, run by an Eventor every 3 s, into the SunQueen.
// Needs a spawned Saturn.

#include "gled_globals.C"

using namespace gled;

void monitor()
{
  ASSERT_MACRO(gled_globals);

  Eventor* top_mon = new Eventor("Cluster Monitor");
  top_mon->SetStampInterval(1);
  top_mon->SetInterBeatMS(3*1000);
  top_mon->SetMultix(true);
  g_sun_queen->CheckIn(top_mon); g_sun_queen->Add(top_mon);

  GledMonitor* gm = new GledMonitor("Node status collector");
  gm->SetFillHistos(true);
  g_sun_queen->CheckIn(gm); top_mon->Add(gm);

  top_mon->Start();
}
