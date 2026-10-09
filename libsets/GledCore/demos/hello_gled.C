// hello_gled.C
// top-level Saturn startup script

#include "sun_demos.C"
#include "eye.C"

using namespace gled;

void hello_gled()
{
  // ROOT usually cacthes segvs ... and then dies horribly anyway.
  // Use this to get your core:
  // gSystem->IgnoreSignal(kSigSegmentationViolation, true);

  ASSERT_MACRO(sun_demos);

  Gled::theOne->AssertLibSet("Geom1");

  Scene* hello_scene = new Scene("Hello Gled");
  g_queen->CheckIn(hello_scene);
  g_queen->Add(hello_scene);
  g_scene = hello_scene;

  Rect* base_plane = new Rect("BasePlane");
  base_plane->SetUnitSquare(20);
  g_queen->CheckIn(base_plane);
  hello_scene->Add(base_plane);

  SMorph* smorph = new SMorph();
  smorph->SetUseScale(true);
  smorph->SetScales(4, 2, 1);
  smorph->SetCx(0.5);
  smorph->SetColor(0.2, 1, 0.2);
  smorph->SetTLevel(16); smorph->SetPLevel(16);
  g_queen->CheckIn(smorph); hello_scene->Add(smorph);

  ZNode* node1 = new ZNode("First Node");
  g_queen->CheckIn(node1); hello_scene->Add(node1);
  Sphere* sph1 = new Sphere("First Sphere");
  sph1->MoveLF(1,5);
  sph1->SetColor(1,0.3,0.3); sph1->SetLOD(16);
  g_queen->CheckIn(sph1);
  node1->Add(sph1);

  ZNode* node2 = new ZNode("Second Node");
  g_queen->CheckIn(node2); hello_scene->Add(node2);
  Sphere* sph2 = new Sphere("Second Sphere");
  sph2->MoveLF(1,-5);
  sph2->SetColor(0.3,0.3,1); sph2->SetLOD(16);
  g_queen->CheckIn(sph2);
  node2->Add(sph2);

  Eventor* dynamo = new Eventor("Dynamo");
  dynamo->SetBeatsToDo(-1);
  dynamo->SetStampInterval(10);
  dynamo->SetInterBeatMS(50);
  g_queen->CheckIn(dynamo); hello_scene->Add(dynamo);

  Mover* mover1 = new Mover("Rotates first node");
  mover1->SetNode(node1);
  mover1->SetRi(1); mover1->SetRj(2); mover1->SetRa(0.01745);
  g_queen->CheckIn(mover1); dynamo->Add(mover1);

  Mover* mover2 = new Mover("Rotates second node");
  mover2->SetNode(node2);
  mover2->SetRi(3); mover2->SetRj(1); mover2->SetRa(0.01745/2);
  g_queen->CheckIn(mover2); dynamo->Add(mover2);

  eye(); // spawn an eye with a pupil on first scene
  setup_pupil_up_reference();

  dynamo->Start();
}
