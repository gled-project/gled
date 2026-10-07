// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "GSpinLock.h"
#include <Gled/GledTypes.h>
#include <errno.h>

using namespace gled;


//______________________________________________________________________________
//
// POSIX spin-lock wrapper class.
//

GSpinLock::GSpinLock()
{
#ifdef __APPLE__
  mSpinLock = 0;
#else
  int ret = pthread_spin_init(&mSpinLock, PTHREAD_PROCESS_PRIVATE);
  if (ret) {
    perror("GSpinLock::GSpinLock");
  }
#endif
}

GSpinLock::~GSpinLock()
{
#ifndef __APPLE__
  int ret = pthread_spin_destroy(&mSpinLock);
  if (ret) {
    perror("GSpinLock::~GSpinLock");
  }
#endif
}

GSpinLock::Lock_e GSpinLock::Lock()
{
#ifdef __APPLE__
  OSSpinLockLock(&mSpinLock);
  return ok;
#else
  int ret = pthread_spin_lock(&mSpinLock);
  switch (ret) {
  case 0:	return ok;
  case EDEADLK:	return deadlock;
  default:	return bad_init;
  }
#endif
}

GSpinLock::Lock_e GSpinLock::TryLock()
{
#ifdef __APPLE__
  return OSSpinLockTry(&mSpinLock) ? ok : busy;
#else
  int ret = pthread_spin_trylock(&mSpinLock);
  switch (ret) {
  case 0:	return ok;
  case EBUSY:	return busy;
  default:	return bad_init;
  }
#endif
}

GSpinLock::Lock_e GSpinLock::Unlock()
{
#ifdef __APPLE__
  OSSpinLockUnlock(&mSpinLock);
  return ok;
#else
  int ret = pthread_spin_unlock(&mSpinLock);
  switch(ret) {
  case 0:	return ok;
  case EPERM:	return perm_fail;
  default:	return bad_init;
  }
#endif
}
