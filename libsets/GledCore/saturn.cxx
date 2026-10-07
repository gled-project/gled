// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

// saturn ... non-GUI gled (just spawns Gled and CINT)
//	      usefull for pure servers / proxies or computing clients

#include <Gled/GledTypes.h>
#include <Gled/GledNS.h>
#include <Gled/Gled.h>
#include <Gled/GThread.h>
#include <Gled/GCondition.h>

#include <Getline.h>

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>

using namespace gled;


/**************************************************************************/

int main(int argc, char **argv)
{
  Gled* gled = new Gled();
  gled->ReadArguments(argc, argv);
  gled->ParseArguments(true);

  if (gled->GetQuit())
  {
    exit(0);
  }

  if (gled->GetIsDaemon())
  {
    printf("saturn - daemonizing ...\n");
    if (daemon(1, 1))
    {
      perror("saturn - daemonization failed");
      exit(1);
    }
  }

  gled->Initialize();

  GCondition gled_exit;
  gled_exit.Lock();
  gled->SetExitCondVar(&gled_exit);

  // Run Root Application thread
  GThread *app_thread = gled->SpawnRootAppThread("saturn.cxx");

  gled_exit.Wait();
  gled_exit.Unlock();

  if (gled->GetRootAppRunning())
  {
    app_thread->Kill(GThread::SigTERM);
  }
  else
  {
    Getlinem(kCleanUp, 0);
  }
  app_thread->Join();

  gled->StopLogging();
  delete gled;

  GThread::FiniMain();

  exit(Gled::GetExitStatus());
}
