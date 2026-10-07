// Does the whole loader chain resolve: base + View + Rnr_GL, for Var1's deps?
#include <Gled/GledNS.h>

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

void deptest()
{
  Gled::theOne->AssertLibSet("Var1");
  show("after AssertLibSet(Var1)");
}
