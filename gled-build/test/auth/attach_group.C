// Run by a client of run_auth.sh: asks the sun for the group identity @test.
#include <Gled/GledNS.h>

using namespace gled;

void attach_group()
{
  ZSunQueen* sq = Gled::theOne->GetSaturn()->GetSunQueen();
  ZIdentity* g  = dynamic_cast<ZIdentity*>(sq->FindLensByPath("Auth/Groups/@test"));
  if (g == 0)
  {
    printf("ATTACH no group @test\n");
    return;
  }
  std::unique_ptr<ZMIR>    mir(sq->S_AttachIdentity(g));
  std::unique_ptr<ZMIR_RR> res(Gled::theOne->GetSaturn()->ShootMIRWaitResult(mir));
  if (res->HasException())
    printf("ATTACH refused: %s\n", res->fException.Data());
  else
    printf("ATTACH granted\n");
}
