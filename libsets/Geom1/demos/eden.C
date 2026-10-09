// Eden: a RectTerrain from the histogram of RectTerrain::Edenify(), colored
// with terrain.pov. The Edenify button in the terrain's GUI does it again.
//
// vars: ZQueen* g_queen
// libs: Geom1

#include <glass_defines.h>
#include <gl_defines.h>

#include "sun_demos.C"
#include "eye.C"

using namespace gled;

void eden()
{
  ASSERT_MACRO(sun_demos);
  Gled::theOne->AssertLibSet("Geom1");

  Scene* eden = new Scene("Eden");
  g_queen->CheckIn(eden);
  g_queen->Add(eden);
  g_scene = eden;

  CREATE_ADD_GLASS(lamp, Lamp, eden, "Lamp", 0);
  lamp->SetDiffuse(0.9, 0.9, 0.85);
  lamp->SetPos(-10, -20, 30);
  eden->GetGlobLamps()->Add(lamp);

  CREATE_ADD_GLASS(ribbon, RGBAPalette, eden, "Terrain Ribbon", 0);
  ribbon->SetMarksFromPOVFile("terrain.pov");

  CREATE_ADD_GLASS(terrain, RectTerrain, eden, "Terrain", 0);
  terrain->SetDx(0.1); terrain->SetDy(0.1);
  terrain->SetPos(-6.4, -6.4, 0);
  terrain->SetScales(1, 1, 3); terrain->SetUseScale(true);
  terrain->SetRibbon(ribbon);
  terrain->SetRnrMode(RectTerrain::RM_SmoothTring);
  terrain->Edenify();

  CREATE_ADD_GLASS(camera, ZNode, eden, "Camera", 0);
  camera->SetPos(-8.5, -7, 5.5);
  camera->SetRotByDegrees(40, -30, 0);

  //============================================================================
  // Spawn GUI

  if (Gled::theOne->HasGUILibs())
  {
    eye();
    setup_pupil_up_reference();
    g_pupil->SetCameraBase(camera);
    g_pupil->EmitCameraHomeRay();
  }
}
