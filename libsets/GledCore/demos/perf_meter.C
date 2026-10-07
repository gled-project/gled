// Simple set-up for performance measurements.
// Spawns Eye automatically.
//
// vars: ZQueen* g_queen

#include "sun_demos.C"
#include "eye.C"

//#include "Ephra/Saturn.h"

using namespace gled;

PipeEventor* client_executor = 0;

void perf_meter()
{
  ASSERT_MACRO(sun_demos);

  Scene* pm_scene  = new Scene("Performance Meters");
  g_queen->CheckIn(pm_scene);
  g_queen->Add(pm_scene);

  client_executor = new PipeEventor("Client Executor");
  const char*  client_host     = "pcalice122";

  client_executor->SetCommand
    (
     GForm("ssh %s ~/bin/gled_run.sh gr1 hereenv.sh %s 9061 > %s 2>&1",
	   client_host,
	   g_saturn->GetSaturnInfo()->GetHostName(),
	   client_host)
     );
  client_executor->SetWaitTimeMS(1000);
  g_queen->CheckIn(client_executor); pm_scene->Add(client_executor);

  PerfMeterTarget* pmt = new PerfMeterTarget("PMTarget");
  g_queen->CheckIn(pmt); pm_scene->Add(pmt);

  // Eventor and PerfMeterOperator

  Eventor *dynamo = new Eventor;
  dynamo->SetBeatsToDo(100000);
  dynamo->SetStampInterval(100000);
  dynamo->SetInterBeatMS(0);
  g_queen->CheckIn(dynamo); pm_scene->Add(dynamo);

  PerfMeterOperator* pmo = new PerfMeterOperator("PMOperator");
  pmo->SetTarget(pmt);
  g_queen->CheckIn(pmo); dynamo->Add(pmo);

  printf("Loading perf_functions.CC\n");
  gROOT->LoadMacro("perf_functions.CC");

  g_scene = pm_scene;
  eye(false);
}
