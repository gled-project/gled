// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ZLog.h"

#include "Gled/GThread.h"

#include "TSystem.h"

#include <time.h>

using namespace gled;

#include "ZLog.c7"

// ZLog

//______________________________________________________________________________
//
// Logs into given file, supports file rotation.
//
// The calling threads format the lines and put them into a queue. A dedicated
// thread writes them out, flushing after each batch, and checks for log
// rotation: every 10 seconds, and when RotateLog() asks for it.
//
// Things I though, at some point, are relevant:
// - GUI (yes, right) -- to replace "main logger".

//==============================================================================

void ZLog::_init()
{
  mFileName = mName + ".log";
  mLevel = L_Message;
  mDebugLevel = 0;

  mLoggerThread = 0;
  bStopLogger = false;
  bCheckRotation = false;
}

ZLog::ZLog(const Text_t* n, const Text_t* t) :
  ZGlass(n, t)
{
  _init();
}

ZLog::~ZLog()
{}

//==============================================================================

// void ZLog::operator()(Int_t level, const char* sth)
// {
//   clock_t clk = clock();
//   printf("Clocka %lu %lu\n", clk, clk / CLOCKS_PER_SEC);
//   printf("Jebojebo, got lvl=%d, txt='%s'\n", level, sth);
// }

//==============================================================================

void ZLog::StartLogging()
{
  static const Exc_t _eh("ZLog::StartLogging ");

  {
    GMutexHolder _lck(mLoggerCond);

    if (mLoggerThread != 0)
      throw _eh + "Logging already active.";

    mStream.open(mFileName, std::ios_base::out | std::ios_base::app);
    if (mStream.fail())
      throw _eh + "Opening of log '" + mFileName + "' failed.";

    mStream << "******************** Logging started at " << GTime::ApproximateTime().ToDateTimeLocal(false) << " ********************" << std::endl;

    bStopLogger = false;
    bCheckRotation = false;
    mLoggerThread = new GThread("ZLog-LogLoop", (GThread_foo) tl_LogLoop, this);
    mLoggerThread->SetNice(20);
    mLoggerThread->Spawn();
  }

  {
    GLensReadHolder _rdlck(this);
    bLogActive = true;
    Stamp(FID());
  }
}

void ZLog::StopLogging()
{
  static const Exc_t _eh("ZLog::StopLogging ");

  GThread *thr;
  {
    GMutexHolder _lck(mLoggerCond);

    if ( ! GThread::IsValidPtr(mLoggerThread))
      throw _eh + "Logging not active.";

    thr = mLoggerThread;
    GThread::InvalidatePtr(mLoggerThread);
    bStopLogger = true;
    mLoggerCond.Signal();
  }

  // The logger thread writes out the queue before it exits.
  thr->Join();

  {
    GMutexHolder _lck(mLoggerCond);

    mStream << "******************** Logging stopped at " << GTime::ApproximateTime().ToDateTimeLocal(false) << " ********************" << std::endl;
    mStream.close();
    mLoggerThread = 0;
    bStopLogger = false;
  }

  {
    GLensReadHolder _rdlck(this);
    bLogActive = false;
    Stamp(FID());
  }

}

void ZLog::RotateLog()
{
  static const Exc_t _eh("ZLog::RotateLog ");

  GMutexHolder _lck(mLoggerCond);

  if ( ! GThread::IsValidPtr(mLoggerThread))
    throw _eh + "Logging not active.";

  if (gSystem->AccessPathName(mFileName, kFileExists) == false)
  {
    TString newfile = mFileName + "." + GTime::ApproximateTime().ToDateLocal();
    if (gSystem->AccessPathName(newfile, kFileExists) == false)
    {
      Int_t cnt = 1;
      newfile += ".";
      TString tstr;
      do
      {
        tstr = newfile;
        tstr += cnt++;
      }
      while (gSystem->AccessPathName(tstr, kFileExists) == false);
      newfile = tstr;
    }
    gSystem->Rename(mFileName, newfile);
  }

  bCheckRotation = true;
  mLoggerCond.Signal();
}

//------------------------------------------------------------------------------

void ZLog::tl_LogLoop(ZLog* log)
{
  log->LogLoop();
}

void ZLog::LogLoop()
{
  // Writes out the queued lines and takes care of log rotation. Exits when
  // StopLogging() asks for it, after writing out the queue.

  static const Exc_t _eh("ZLog::LogLoop ");

  std::list<TString> lines;
  GTime next_check = GTime::Now() + GTime(10, 0);

  while (true)
  {
    bool check, stop;
    {
      GMutexHolder _lck(mLoggerCond);
      while (mQueue.empty() && ! bStopLogger && ! bCheckRotation)
      {
        if (mLoggerCond.TimedWaitUntil(next_check) == 1)
          bCheckRotation = true;
      }
      lines.swap(mQueue);
      check = bCheckRotation;
      stop  = bStopLogger;
      bCheckRotation = false;
    }

    // Only this thread writes to the stream while it runs.
    write_queue(lines);

    if (check)
    {
      next_check = GTime::Now() + GTime(10, 0);

      if (gSystem->AccessPathName(mFileName, kFileExists) == true)
      {
        TString time(GTime::ApproximateTime().ToDateTimeLocal(false));
        mStream << "******************** Logging rotated at " << time << " ********************" << std::endl;
        mStream.close();
        mStream.open(mFileName, std::ios_base::out | std::ios_base::app);
        if (mStream.fail())
        {
          ISerr(_eh + "Opening of log '" + mFileName + "' failed. Requesting logging termination.");
          mSaturn->ShootMIR(S_StopLogging());
        }
        mStream << "******************** Logging rotated at " << time << " ********************" << std::endl;
      }
    }

    if (stop)
      break;
  }
}

void ZLog::write_queue(std::list<TString>& lines)
{
  if (lines.empty())
    return;

  for (auto& l : lines)
    mStream << l << '\n';
  mStream.flush();
  lines.clear();
}

void ZLog::enqueue(const TString& line)
{
  // Lines put while logging is not active are dropped.

  GMutexHolder _lck(mLoggerCond);
  if ( ! GThread::IsValidPtr(mLoggerThread))
    return;
  mQueue.push_back(line);
  mLoggerCond.Signal();
}

//==============================================================================

namespace
{
  static const char *lvl_names[] = { "FTL", "ERR", "WRN", "MSG", "INF", "DBG" };
}

#define LEVEL_CHECK(_lvl_) \
  if (mLevel < L_Debug) { \
    if (_lvl_ > mLevel) return; \
  } else { \
    if (_lvl_ > L_Debug + mDebugLevel) return; \
  }

#define LEVEL_NAME(_name_, _lvl_) \
  TString _name_; \
  if (_lvl_ <= L_Debug) { \
    _name_ = lvl_names[_lvl_]; \
  } else { \
    _name_ = lvl_names[L_Debug]; \
    _name_ += char('0' + _lvl_ - L_Debug); \
  }

void ZLog::Put(Int_t level, const TString& prefix, const TString& message)
{
  Put(GTime::ApproximateTime().ToDateTimeLocal(false), level, prefix, message);
}

void ZLog::Put(const GTime& time, Int_t level, const TString& prefix, const TString& message)
{
  Put(time.ToDateTimeLocal(false), level, prefix, message);
}

void ZLog::Put(const TString& time_string, Int_t level, const TString& prefix, const TString& message)
{
  LEVEL_CHECK(level);
  LEVEL_NAME(lvl_name, level);
  TString pim(prefix.EndsWith(" ") ? "" : " ");

  enqueue(time_string + " " + lvl_name + " " + prefix + pim + message);
}

void ZLog::Form(Int_t level, const TString& prefix, const char* fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);
  FormVA(GTime::ApproximateTime().ToDateTimeLocal(false), level, prefix, fmt, ap);
  va_end(ap);
}

void ZLog::Form(const GTime& time, Int_t level, const TString& prefix, const char* fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);
  FormVA(time.ToDateTimeLocal(false), level, prefix, fmt, ap);
  va_end(ap);
}

void ZLog::Form(const TString& time_string, Int_t level, const TString& prefix, const char* fmt, ...)
{
  LEVEL_CHECK(level);
  LEVEL_NAME(lvl_name, level);
  TString message;
  {
    va_list ap;
    va_start(ap, fmt);
    message = GFormVA(fmt, ap);
    va_end(ap);
  }
  TString pim(prefix.EndsWith(" ") ? "" : " ");

  enqueue(time_string + " " + lvl_name + " " + prefix + pim + message);
}

void ZLog::FormVA(const GTime& time, Int_t level, const TString& prefix, const char* fmt, va_list args)
{
  FormVA(time.ToDateTimeLocal(false), level, prefix, fmt, args);
}

void ZLog::FormVA(const TString& time_string, Int_t level, const TString& prefix, const char* fmt, va_list args)
{
  LEVEL_CHECK(level);
  LEVEL_NAME(lvl_name, level);
  TString message(GFormVA(fmt, args));
  TString pim(prefix.EndsWith(" ") || prefix.IsNull() ? "" : " ");

  enqueue(time_string + " " + lvl_name + " " + prefix + pim + message);
}


//==============================================================================
// ZLog::Helper
//==============================================================================

ZLog::Helper::Helper(ZLog* log, const TString& pfx) :
  m_log(log), m_prefix(pfx), m_level(L_Message)
{
  SetTime(GTime::ApproximateTime());
}

ZLog::Helper::Helper(ZLog* log, Int_t lvl, const TString& pfx) :
  m_log(log), m_prefix(pfx), m_level(lvl)
{
  SetTime(GTime::ApproximateTime());
}

ZLog::Helper::Helper(ZLog* log, const GTime& when, const TString& pfx) :
  m_log(log), m_prefix(pfx), m_level(L_Message)
{
  SetTime(when);
}

ZLog::Helper::Helper(ZLog* log, const GTime& when, Int_t lvl, const TString& pfx) :
  m_log(log), m_prefix(pfx), m_level(lvl)
{
  SetTime(when);
}

void ZLog::Helper::SetTime(const GTime& time)
{
  m_time = time;
  m_time_string = m_time.ToDateTimeLocal(false);
}

void ZLog::Helper::Put(const TString& message)
{
  if (m_log)
  {
    m_log->Put(m_time_string, m_level, m_prefix, message);
  }
}

void ZLog::Helper::Put(Int_t level, const TString& message)
{
  if (m_log)
  {
    m_log->Put(m_time_string, level, m_prefix, message);
  }
}

void ZLog::Helper::Form(const char* fmt, ...)
{
  if (m_log)
  {
    va_list ap;
    va_start(ap, fmt);
    m_log->FormVA(m_time_string, m_level, m_prefix, fmt, ap);
    va_end(ap);
  }
}

void ZLog::Helper::Form(Int_t level, const char* fmt, ...)
{
  if (m_log)
  {
    va_list ap;
    va_start(ap, fmt);
    m_log->FormVA(m_time_string, level, m_prefix, fmt, ap);
    va_end(ap);
  }
}
