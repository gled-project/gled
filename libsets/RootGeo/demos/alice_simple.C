// alice_simple: simplified ALICE geometry for event visualization
//
// libs: Geom1, RootGeo

#include <glass_defines.h>
#include <gl_defines.h>

#include "sun_demos.C"

using namespace gled;

Scene* alice_simple_init(const Text_t* geom_file, const Text_t* det_file);

ZGeoNode* g_simple_geometry = 0;

#include "common_foos.C"

/**************************************************************************/
// main
/**************************************************************************/

void alice_simple(const Text_t* geom_file = "simple_geo.root",
		  const Text_t* det_file  = 0)
{
  // usage: aligled <options> -- <gled-options> alice_simple.C

  ASSERT_MACRO(sun_demos);

  Gled::theOne->AssertLibSet("Geom1");
  Gled::theOne->AssertLibSet("RootGeo");

  //--------------------------------------------------------------

  alice_simple_init(geom_file, det_file);

  //--------------------------------------------------------------

  setup_default_gui();
  spawn_default_gui();
}

/**************************************************************************/

Scene* alice_simple_init(const Text_t* geom_file = "simple_geo.root",
			 const Text_t* det_file  = 0)
{
  printf("Importing geometry ...\n");
  TGeoManager::Import(geom_file);
  printf("Done importing geometry.\n");

  //--------------------------------------------------------------

  Scene* rscene = create_basic_scene(); // Returned also in g_scene

  ZGlLightModel* lm = (ZGlLightModel*) rscene->FindLensByPath("Var/Light Model");
  lm->SetLightModelOp(ZRnrModBase::O_Off);
  lm->SetFrontMode(GL_LINE);
  lm->SetBackMode(GL_LINE);
  lm->SetFaceCullOp(ZRnrModBase::O_Off);

  //--------------------------------------------------------------

  ZGeoNode* znode = new ZGeoNode("Geometry");
  znode->SetTNode(gGeoManager->GetTopNode());
  znode->SetOM(-2);
  znode->SetUseOM(true);
  g_queen->CheckIn(znode);
  rscene->Add(znode);
  znode->SetRnrSelf(false);

  if (det_file != 0)
  {
    znode->SetDefFile(det_file);
    znode->LoadFromFile();
    znode->Restore();
  }
  else
  {
    znode->ImportNodesRec(3);
  }

  g_simple_geometry = znode;

  return rscene;
}
