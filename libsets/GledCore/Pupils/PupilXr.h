// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef GledCore_PupilXr_H
#define GledCore_PupilXr_H

namespace gled {

class Pupil;

// An OpenXR session that renders a pupil's scene into a headset, from an
// FLTK idle callback. Started through PupilInfo::XrStart() or GLED_XR.
// The grips move, turn and scale the scene, A recenters, the left menu ends.
// The sticks go to the pupil's event handler through PupilInfo, or fly. The
// right hand's laser picks with the trigger and shoots with B, as mouse 1 and 3.
class PupilXr
{
  struct Impl;
  Impl  *m;

  void frame();
  bool poll_events();
  void update_input(long long display_time);
  void sticks_off();
  void laser_event(int ev, int button);
  void render_eye(int eye);

public:
  PupilXr(Pupil* pupil);
  ~PupilXr();

  // Needs the pupil's GL context current.
  bool Init();
  // False when no session runs; then the caller deletes this.
  bool RequestExit();

  static void IdleCallback(void* pxr);
};

} // endnamespace gled

#endif
