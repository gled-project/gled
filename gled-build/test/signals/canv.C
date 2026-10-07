// What ROOT's TCanvas resolves to under gled: the web display setting, the
// canvas implementation, gVirtualX and the batch flags.
//
//   gled --run --noprompt canv.C                   # TRootCanvas, 'off'
//   gled --run --noprompt --root-web on canv.C     # TWebCanvas
//
// With --root-web server:<port> it is also the start-up case for sigtest.sh
// (pattern 'canvas batch'): TWebCanvas then waits ~30 s for a browser.

#include <Gled/GledNS.h>
using namespace gled;
void canv()
{
  printf("WEBDISPLAY   = '%s'\n", gROOT->GetWebDisplay().Data());
  printf("IsBatch      = %d\n", gROOT->IsBatch());
  TCanvas* c = new TCanvas("c","c",300,200);
  printf("TCanvas impl = %s\n", c->GetCanvasImp() ? c->GetCanvasImp()->IsA()->GetName() : "<null>");
  printf("gVirtualX    = %s  (name '%s')\n", gVirtualX->IsA()->GetName(), gVirtualX->GetName());
  printf("canvas batch = %d\n", c->IsBatch());
}
