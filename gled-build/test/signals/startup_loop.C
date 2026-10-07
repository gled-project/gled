// Ctrl-C during a start-up macro under the TRint prompt.  The macro pumps
// gSystem->ProcessEvents() for 20 s.  Run gled with its prompt on a
// pseudo-terminal and type Ctrl-C while "loop start" is the last line;
// TRint should break out of the macro ("loop end" never printed) and give
// the prompt.
//
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
