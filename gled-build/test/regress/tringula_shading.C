// Run after tringula.C: switches its terrain to smooth shading, and with strips=1
// generates triangle strips and draws with them; see test/README.md.
#include <Gled/GledNS.h>

using namespace gled;

void tringula_shading(Int_t smooth = 1, Int_t strips = 0)
{
  if (strips)
  {
    GLensWriteHolder _wlck(trimesh);
    trimesh->GetTTvor()->GenerateTriangleStrips();
  }
  g_saturn->ShootMIR(trirep->S_SetSmoothShading(smooth));
  g_saturn->ShootMIR(trirep->S_SetTringStrips(strips));
}
