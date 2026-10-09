// Prints what TCanvas resolves to under gled (web display, implementation,
// gVirtualX, batch); the start-up case of sigtest.sh with --root-web server:<port>.

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
