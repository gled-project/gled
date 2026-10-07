// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later


//______________________________________________________________________
// GCondition
//
// pthread condition variable + mutex.
// Lock/Unlock for Signal() and Broadcast() is made automatically,
// for wait operations it *must* be called from outside.

#include "GCondition.h"

#include <errno.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

using namespace gled;

namespace
{
  // Timed waits run on the monotonic clock, so that steps of the wall clock
  // (NTP, suspend and resume) do not shorten or stretch them.

  struct timespec monotonic_deadline(const GTime& interval)
  {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    if (interval.GetSec() > 0 || (interval.GetSec() == 0 && interval.GetNSec() > 0))
    {
      ts.tv_sec  += interval.GetSec();
      ts.tv_nsec += interval.GetNSec();
      if (ts.tv_nsec >= 1000000000)
      {
        ts.tv_sec  += 1;
        ts.tv_nsec -= 1000000000;
      }
    }
    return ts;
  }
}


//______________________________________________________________________________
//
// POSIX condition-variable wrapper class.
//
// Inherits from GMutex so that is automatically associated with this
// condition. So, simply lock/unlock the condition variable object to
// ensure that no signals are missed.
//

/**************************************************************************/

GCondition::GCondition(GMutex::Init_e e) : GMutex(e)
{
  pthread_condattr_t attr;
  pthread_condattr_init(&attr);
  pthread_condattr_setclock(&attr, CLOCK_MONOTONIC);
  pthread_cond_init(&mCond, &attr);
  pthread_condattr_destroy(&attr);
}

GCondition::~GCondition()
{
  pthread_cond_destroy(&mCond);
}

/**************************************************************************/

Int_t GCondition::Wait()
{
  // Performs wait ... mutex should be locked upon calling this method.

  int ret = pthread_cond_wait(&mCond, &mMut);
  return ret;
}

Int_t GCondition::TimedWait(GTime time)
{
  // Performs timedwait for interval time.
  // Mutex should be locked upon calling this method.
  // Returns 1 for time-out, 0 for other cases.

  struct timespec timeout = monotonic_deadline(time);

  return pthread_cond_timedwait(&mCond, &mMut, &timeout) == ETIMEDOUT ? 1 : 0;
}

Int_t GCondition::TimedWaitUntil(GTime time)
{
  // Performs timedwait until time, a wall-clock time as given by GTime::Now().
  // The remaining interval is measured on the monotonic clock.
  // Mutex should be locked upon calling this method.
  // Returns 1 for time-out, 0 for other cases.

  // Never is the most negative time and times out at once, as it always did.
  struct timespec timeout = monotonic_deadline(time.IsNever() ? GTime() : time - GTime::Now());

  return pthread_cond_timedwait(&mCond, &mMut, &timeout) == ETIMEDOUT ? 1 : 0;
}

Int_t GCondition::Signal()
{
  int ret = pthread_cond_signal(&mCond);
  return ret;
}

Int_t GCondition::Broadcast()
{
  int ret = pthread_cond_broadcast(&mCond);
  return ret;
}

Int_t GCondition::LockSignal()
{
  Lock();
  int ret = pthread_cond_signal(&mCond);
  Unlock();
  return ret;
}

Int_t GCondition::LockBroadcast()
{
  Lock();
  int ret = pthread_cond_broadcast(&mCond);
  Unlock();
  return ret;
}

