// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "TabletReader.h"
#include "TabletStroke.h"
#include "TabletStrokeList.h"
using namespace gled;
#include "TabletReader.c7"

#include <Glasses/ZQueen.h>


// TabletReader

//______________________________________________________________________________
//
// Turns pen strokes from an evdev tablet into TabletStrokes. Devices and
// permissions: docs/glasses.md.

//==============================================================================

void TabletReader::_init()
{
  bGrab = true;

  mStrokeType = SS_Absolute;
  bInvertY = true;
  bKeepStrokeInProximity = true;

  mPosScale = mPrsScale = 0;
  mOffX = mOffY = 0;

  mButtons = 0;
  bButton0 = bButton1 = bStylus1 = bStylus2 = false;
  bInProximity = bInTouch = bInStroke = false;

  bPrintButtEvs   = false;
  bPrintButtState = false;
  bPrintPositions = false;
  bPrintOther     = true;
}

TabletReader::TabletReader(const Text_t* n, const Text_t* t) :
  ZNode(n, t),
  mTabletThread(0), bReqStop(false)
{
  _init();
}

TabletReader::~TabletReader()
{}

//==============================================================================

const char* TabletReader::get_button_name(Int_t bb)
{
  const char* name = "undef";
  switch (bb)
  {
    case BB_Touch:    name = "Touch";    break;
    case BB_Stylus_1: name = "Stylus_1"; break;
    case BB_Stylus_2: name = "Stylus_2"; break;
    case BB_Button_0: name = "Button_0"; break;
    case BB_Button_1: name = "Button_1"; break;
  }
  return name;
}

Bool_t TabletReader::flip_report_button(Int_t bb)
{
  // Bool_t prev = get_button(bb);
  mButtons ^= bb;
  Bool_t val = get_button(bb);
  if (bPrintButtEvs)
  {
    const char* name = get_button_name(bb);
    printf("Button %s %s\n", name, val ? "DOWN" : "UP");
    // printf("Button %s %s, was %s\n", name, val ? "DOWN" : "UP", prev ? "DOWN" : "UP");
  }
  return val;
}

Bool_t TabletReader::check_pen_buttons(Int_t buttons_delta)
{
  // Check for changes in pen-related buttons.
  // Returns true if there was a change.

  Bool_t change = false;

  if (buttons_delta & BB_Touch)
  {
    Bool_t down = flip_report_button(BB_Touch);
    if ( ! bInStroke || ! bKeepStrokeInProximity)
    {
      if (down)
      {
	begin_stroke();
      }
      else
      {
	end_stroke();
      }
    }
    bInTouch = down;
    change = true;
  }
  if (buttons_delta & BB_Stylus_1)
  {
    bStylus1 = flip_report_button(BB_Stylus_1);
    change = true;
  }
  if (buttons_delta & BB_Stylus_2)
  {
    bStylus2 = flip_report_button(BB_Stylus_2);
    change = true;
  }

  return change;
}

void TabletReader::clear_pen_buttons()
{
  if (bInStroke)
    end_stroke();
  mButtons &= ~BB_Pad_Buttons;
  bInTouch = bStylus1 = bStylus2 = false;
}

Bool_t TabletReader::check_pad_buttons(Int_t buttons_delta)
{
  // Check for changes in pad-related buttons.
  // Returns true if there was a change.

  Bool_t change = false;

  if (buttons_delta & BB_Button_0)
  {
    bool down = flip_report_button(BB_Button_0);
    if (down)
      begin_stroke_list();
    bButton0 = down;
    change = true;
  }
  if (buttons_delta & BB_Button_1)
  {
    bool down = flip_report_button(BB_Button_1);
    if (down)
      end_stroke_list();
    bButton1 = down;
    change = true;
  }

  return change;
}

//==============================================================================

void TabletReader::begin_stroke_list()
{
  static const Exc_t _eh("TabletReader::begin_stroke_list ");

  GLensWriteHolder _wlck(this);

  if (mStrokeList == 0)
  {
    mFirstStrokeStart = mStrokeStart = GTime();

    TabletStrokeList *slist = new TabletStrokeList("StrokeList");
    mQueen->CheckIn(slist);
    SetStrokeList(slist);
    Add(slist);
  }
  else
  {
    ISwarn(_eh + "Already active StrokeList.");
  }
}

void TabletReader::end_stroke_list()
{
  static const Exc_t _eh("TabletReader::end_stroke_list ");

  GLensWriteHolder _wlck(this);

  if (mStrokeList != 0)
  {
    end_stroke();
    SetStrokeList(0);
  }
  else
  {
    ISwarn(_eh + "No active StrokeList.");
  }
}

void TabletReader::begin_stroke()
{
  GLensWriteHolder _wlck(this);

  if (mFirstStrokeStart.IsZero())
    mFirstStrokeStart = mEventTime;
  mStrokeStart = mEventTime;

  if (mStrokeList != 0 && mStroke == 0)
  {
    TabletStroke *stroke = new TabletStroke("Stroke");
    stroke->SetStartTime((mStrokeStart - mFirstStrokeStart).ToFloat());
    mQueen->CheckIn(stroke);
    SetStroke(stroke);
    bInStroke = true;
    {
      GLensReadHolder _rlck(*mStroke);
      mStroke->BeginStroke();
    }
    std::unique_ptr<ZMIR> m(mStrokeList->S_Add(*mStroke));
    mSaturn->ShootMIR(m);
  }
}

void TabletReader::end_stroke()
{
  GLensWriteHolder _wlck(this);

  if (mStroke != 0)
  {
    {
      GLensReadHolder _rlck(*mStroke);
      mStroke->EndStroke(bKeepStrokeInProximity);
    }
    bInStroke = false;
    SetStroke(0);
  }
}


//==============================================================================

#include <cerrno>
#include <cstring>
#include <vector>

#include <dirent.h>
#include <fcntl.h>
#include <linux/input.h>
#include <poll.h>
#include <sys/ioctl.h>
#include <unistd.h>

namespace
{
  const int k_long_bits = 8 * sizeof(long);

  bool test_bit(const unsigned long* bits, int b)
  {
    return (bits[b / k_long_bits] >> (b % k_long_bits)) & 1;
  }

  struct EvDev
  {
    TString           fPath;
    input_id          fId;
    bool              fIsPen;
    std::vector<int>  fButtons;   // keys outside the BTN_TOOL_* block
  };

  // Opens the device and reads its identity and capabilities; false if the
  // device cannot be opened, e.g. for lack of permission.
  bool probe(const TString& path, EvDev& d)
  {
    int fd = open(path, O_RDONLY | O_NONBLOCK);
    if (fd < 0) return false;

    unsigned long keys[KEY_MAX / k_long_bits + 1] = { 0 };
    unsigned long abss[ABS_MAX / k_long_bits + 1] = { 0 };
    ioctl(fd, EVIOCGID, &d.fId);
    ioctl(fd, EVIOCGBIT(EV_KEY, sizeof(keys)), keys);
    ioctl(fd, EVIOCGBIT(EV_ABS, sizeof(abss)), abss);
    close(fd);

    d.fPath  = path;
    d.fIsPen = test_bit(keys, BTN_TOOL_PEN) && test_bit(abss, ABS_PRESSURE);
    d.fButtons.clear();
    for (int k = BTN_MISC; k < KEY_MAX; ++k)
      if (test_bit(keys, k) && (k < BTN_DIGI || k > BTN_DIGI + 0xf))
        d.fButtons.push_back(k);
    return true;
  }

  std::vector<EvDev> probe_all()
  {
    std::vector<EvDev> devs;
    DIR* dir = opendir("/dev/input");
    if (dir == 0) return devs;
    while (dirent* e = readdir(dir))
    {
      if (strncmp(e->d_name, "event", 5) != 0) continue;
      EvDev d;
      if (probe(TString("/dev/input/") + e->d_name, d))
        devs.push_back(d);
    }
    closedir(dir);
    return devs;
  }

  int open_device(const TString& path, bool grab, const Exc_t& eh)
  {
    int fd = open(path, O_RDONLY | O_NONBLOCK);
    if (fd < 0)
      throw eh + "cannot open '" + path + "': " + strerror(errno) + ".";
    if (grab && ioctl(fd, EVIOCGRAB, 1) != 0)
      printf("%scannot grab '%s': %s.\n", eh.Data(), path.Data(), strerror(errno));
    return fd;
  }
}

void TabletReader::process_pen_report(Int_t pen_buttons, Int_t tool, Int_t x, Int_t y, Int_t p)
{
  // Handles one complete report of the pen device, as the linuxwacom reader
  // handled one state of the tablet.

  if (tool == T_None)
  {
    if (bInProximity)
    {
      clear_pen_buttons();
      mRawX = mRawY = mRawP = -1;
      bInProximity = false;
      StampReqTring(FID());
    }
    return;
  }

  bInProximity = true;

  Int_t buttons_delta = (pen_buttons ^ mButtons) & BB_Pad_Buttons;
  check_pen_buttons(buttons_delta);

  if (mRawX != x || mRawY != y || mRawP != p)
  {
    mRawX = x; mRawY = y; mRawP = p;

    mPenX = mPosScale * (mRawX - mOffX);
    mPenY = mPosScale * (mRawY - mOffY);
    mPenP = mPrsScale * mRawP;
    mPenT = (mEventTime - mStrokeStart).ToFloat();
    if (bInvertY) mPenY = -mPenY;
    if (bPrintPositions)
    {
      printf("PEN tool=%s x=%d y=%d p=%d\n", tool == T_Eraser ? "eraser" : "pen", x, y, p);
    }
    if (bInStroke)
    {
      GLensReadHolder _lck(*mStroke);
      mStroke->AddPoint(mPenX, mPenY, mPenT, mPenP);
      mStroke->StampReqTring(TabletStroke::FID());
    }
  }

  if (bPrintButtState)
  {
    printf("S1:%d/%d, S2:%d/%d, B0:%d/%d, B1:%d/%d\n",
           bStylus1, get_button(BB_Stylus_1),
           bStylus2, get_button(BB_Stylus_2),
           bButton0, get_button(BB_Button_0),
           bButton1, get_button(BB_Button_1));
  }
  StampReqTring(FID());
}

void TabletReader::StartRead()
{
  static const Exc_t _eh("TabletReader::StartRead ");

  TString pen_path, pad_path;
  Bool_t  grab;
  {
    GLensReadHolder _rlck(this);
    pen_path = mPenDevice;
    pad_path = mPadDevice;
    grab     = bGrab;
  }

  // Find the devices.
  EvDev pen, pad;
  bool  have_pen = false, have_pad = false;
  if (pen_path.IsNull() || pad_path.IsNull())
  {
    std::vector<EvDev> devs = probe_all();
    if (pen_path.IsNull())
    {
      for (auto& d : devs) if (d.fIsPen) { pen = d; have_pen = true; break; }
    }
    if (pad_path.IsNull() && have_pen)
    {
      for (auto& d : devs)
      {
        if (d.fPath != pen.fPath && ! d.fIsPen && ! d.fButtons.empty() &&
            d.fId.vendor == pen.fId.vendor && d.fId.product == pen.fId.product)
        {
          pad = d; have_pad = true; break;
        }
      }
    }
  }
  if ( ! pen_path.IsNull()) have_pen = probe(pen_path, pen);
  if ( ! pad_path.IsNull()) have_pad = probe(pad_path, pad);
  if ( ! have_pen)
    throw _eh + "no tablet pen found; no readable input device reports a pen with pressure.";

  int pen_fd, pad_fd = -1;
  {
    GMutexHolder _lck(mTabletMutex);
    if (mTabletThread != 0)
    {
      throw _eh + "Thread already running.";
    }
    pen_fd = open_device(pen.fPath, grab, _eh);
    if (have_pad)
    {
      try { pad_fd = open_device(pad.fPath, grab, _eh); }
      catch (Exc_t&) { close(pen_fd); throw; }
    }
    mTabletThread = GThread::Self();
  }

  char name[256] = "";
  ioctl(pen_fd, EVIOCGNAME(sizeof(name)), name);
  input_absinfo ax, ay, ap;
  ioctl(pen_fd, EVIOCGABS(ABS_X), &ax);
  ioctl(pen_fd, EVIOCGABS(ABS_Y), &ay);
  ioctl(pen_fd, EVIOCGABS(ABS_PRESSURE), &ap);
  printf("%spen '%s' on %s, x %d..%d, y %d..%d, pressure %d..%d; pad %s.\n",
         _eh.Data(), name, pen.fPath.Data(), ax.minimum, ax.maximum,
         ay.minimum, ay.maximum, ap.minimum, ap.maximum,
         have_pad ? pad.fPath.Data() : "none");

  // The first two pad buttons, in the order of their key codes.
  int pad_b0 = have_pad && pad.fButtons.size() > 0 ? pad.fButtons[0] : -1;
  int pad_b1 = have_pad && pad.fButtons.size() > 1 ? pad.fButtons[1] : -1;

  mPosScale = 1.0f / (ax.maximum - ax.minimum);
  mOffX     = (ax.minimum + ax.maximum) / 2;
  mOffY     = (ay.minimum + ay.maximum) / 2;
  mPrsScale = 1.0f / (ap.maximum - ap.minimum);
  Stamp(FID());

  mButtons = 0;
  bButton0 = bButton1 = bStylus1 = bStylus2 = false;
  bInProximity = bInTouch = bInStroke = false;

  mRawX = mRawY = mRawP = -1;

  mPenXOff = mPenYOff = mPenTOff = 0;

  // StopRead() interrupts poll() with SIGUSR1; the timeout covers a signal
  // that arrives before poll() is entered.
  GThread::UnblockSignal(GThread::SigUSR1);

  // Pen state, applied at each report.
  Int_t x = 0, y = 0, p = 0, tool = T_None, pen_buttons = 0;

  while ( ! bReqStop)
  {
    pollfd pfd[2] = { { pen_fd, POLLIN, 0 }, { pad_fd, POLLIN, 0 } };
    int n = poll(pfd, pad_fd >= 0 ? 2 : 1, 250);
    if (n < 0 && errno != EINTR)
    {
      printf("%spoll failed: %s; stopping.\n", _eh.Data(), strerror(errno));
      break;
    }
    if (n <= 0) continue;
    if ((pfd[0].revents | (pad_fd >= 0 ? pfd[1].revents : 0)) & (POLLERR | POLLHUP | POLLNVAL))
    {
      printf("%stablet device gone; stopping.\n", _eh.Data());
      break;
    }

    input_event ev;

    while (read(pen_fd, &ev, sizeof(ev)) == (ssize_t) sizeof(ev))
    {
      if (ev.type == EV_ABS)
      {
        if      (ev.code == ABS_X)        x = ev.value;
        else if (ev.code == ABS_Y)        y = ev.value;
        else if (ev.code == ABS_PRESSURE) p = ev.value;
      }
      else if (ev.type == EV_KEY)
      {
        Int_t bb = 0;
        switch (ev.code)
        {
          case BTN_TOUCH:       bb = BB_Touch;    break;
          case BTN_STYLUS:      bb = BB_Stylus_1; break;
          case BTN_STYLUS2:     bb = BB_Stylus_2; break;
          case BTN_TOOL_PEN:    tool = ev.value ? T_Pen    : T_None; break;
          case BTN_TOOL_RUBBER: tool = ev.value ? T_Eraser : T_None; break;
          default:
            if (bPrintOther) printf("%spen device key %d = %d\n", _eh.Data(), ev.code, ev.value);
        }
        if (bb) pen_buttons = ev.value ? (pen_buttons | bb) : (pen_buttons & ~bb);
      }
      else if (ev.type == EV_SYN && ev.code == SYN_REPORT)
      {
        mEventTime.SetNow();
        process_pen_report(pen_buttons, tool, x, y, p);
      }
    }

    if (pad_fd >= 0)
    {
      while (read(pad_fd, &ev, sizeof(ev)) == (ssize_t) sizeof(ev))
      {
        if (ev.type != EV_KEY) continue;
        Int_t bb = ev.code == pad_b0 ? BB_Button_0 : (ev.code == pad_b1 ? BB_Button_1 : 0);
        if (bb == 0) continue;
        if (( ! get_button(bb)) != ( ! ev.value))
        {
          mEventTime.SetNow();
          check_pad_buttons(bb);
          StampReqTring(FID());
        }
      }
    }
  }

  {
    GMutexHolder _lck(mTabletMutex);

    close(pen_fd);
    if (pad_fd >= 0) close(pad_fd);
    printf("%stablet closed.\n", _eh.Data());

    bReqStop = false;
    mTabletThread = 0;
  }
}

void TabletReader::StopRead()
{
  static const Exc_t _eh("TabletReader::StopRead ");

  GMutexHolder _lck(mTabletMutex);

  if (mTabletThread == 0)
    throw _eh + "Thread not running.";
  if (bReqStop)
    throw _eh + "Already requested.";

  bReqStop = true;

  mTabletThread->Kill(GThread::SigUSR1);
}
