// Ctrl-C during a start-up macro under the prompt; TRint must break out of it.
//   ctrlc_pty.py out.txt 'loop start' --logflush startup_loop.C

#include <Gled/GledNS.h>
using namespace gled;

void startup_loop()
{
  printf("loop start\n");
  for (int i = 0; i < 200; ++i)
  {
    gSystem->ProcessEvents();
    gSystem->Sleep(100);
  }
  printf("loop end\n");
}
