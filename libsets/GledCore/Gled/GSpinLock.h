// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef GledCore_GSpinLock_H
#define GledCore_GSpinLock_H

#include <Rtypes.h>
 #ifdef __APPLE__
  #include <libkern/OSAtomic.h>
 #else
  #include <pthread.h>
 #endif

namespace gled {

/**************************************************************************/
// GSpinLock
/**************************************************************************/

class GSpinLock
{
protected:
 #ifdef __APPLE__
  OSSpinLock            mSpinLock;
 #else
  pthread_spinlock_t	mSpinLock;
 #endif

public:
  enum Lock_e { ok=0, bad_init, deadlock, busy, perm_fail };

  GSpinLock();
  ~GSpinLock();

  Lock_e Lock();
  Lock_e TryLock();
  Lock_e Unlock();

  ClassDefNV(GSpinLock, 0);
}; // endclass GSpinLock


/**************************************************************************/
// GSpinLockHolder / AntiHolder
/**************************************************************************/

class GSpinLockHolder
{
  GSpinLock& mMutex;
public:
  GSpinLockHolder(GSpinLock& m) : mMutex(m) { mMutex.Lock();   }
  virtual ~GSpinLockHolder()                { mMutex.Unlock(); }

  ClassDef(GSpinLockHolder, 0);
};

// Usability of AntiHolder limited to cases when you're sure
// the mutex is locked exactly once.

class GSpinLockAntiHolder
{
  GSpinLock& mMutex;
public:
  GSpinLockAntiHolder(GSpinLock& m) : mMutex(m) { mMutex.Unlock();   }
  virtual ~GSpinLockAntiHolder()             { mMutex.Lock(); }

  ClassDef(GSpinLockAntiHolder, 0);
};

} // endnamespace gled

#endif
