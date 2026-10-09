// Checks in Geom1 and Var1 lenses without AssertLibSet; see test/README.md.
#include <Gled/GledNS.h>

#include "sun_demos.C"
#include "eye.C"

using namespace gled;

void show(const char* tag)
{
  const char* ls[] = { "GledCore","Numerica","Audio1","Geom1","GledGTS","Var1", 0 };
  printf("=== %s ===\n", tag);
  printf("    %-9s %-6s %-6s %s\n", "libset", "base", "View", "Rnr_GL");
  for (int i = 0; ls[i]; ++i)
  {
    TString v = TString(ls[i]) + "_GLED_init_View";
    TString r = TString(ls[i]) + "_GLED_init_Rnr_GL";
    printf("    %-9s %-6s %-6s %s\n", ls[i],
           GledNS::IsLoaded(ls[i])            ? "yes" : "NO",
           GledNS::FindSymbol(v) ? "yes" : "no",
           GledNS::FindSymbol(r) ? "yes" : "no");
  }
}

void noassert()
{
  ASSERT_MACRO(sun_demos);

  Scene* sc = new Scene("NoAssert");
  g_queen->CheckIn(sc);
  g_queen->Add(sc);
  g_scene = sc;

  Rect* r = new Rect("Rect from Geom1");
  r->SetUnitSquare(10);
  g_queen->CheckIn(r);
  sc->Add(r);

  TriMesh* tm = new TriMesh("TriMesh from Var1");
  g_queen->CheckIn(tm);
  sc->Add(tm);

  show("after checking in a Rect and a TriMesh");

  eye();
}
