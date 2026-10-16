// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef GledCore_PupilXr_H
#define GledCore_PupilXr_H

namespace gled {

class Pupil;

// An OpenXR session that renders a pupil's scene into a headset, from an
// FLTK idle callback. Started through PupilInfo::XrStart() or GLED_XR.
// Controls: the grips move, turn and scale the scene (XrRig), A recenters,
// the left menu button ends. The sticks go to the pupil's event handler
// through PupilInfo, or fly. The right hand's laser picks with the trigger
// and shoots with B, as mouse buttons 1 and 3; on the overlay panel above
// the left hand, which X shows and hides, it works the overlay's widgets.
// The pupil's window mirrors the left eye, unless GLED_XR_DESKTOP is set.
class PupilXr
{
  struct Impl;
  struct Input;
  Impl  *m;

  // Per frame, in this order.
  void frame();
  bool poll_events();
  void read_input(Input& in, long long display_time);
  void act(const Input& in, double dt);
  void render_panel();
  void render_eye(int eye);

  void sticks_off();
  void laser_event(int ev, int button);
  void overlay_event(int ev, int x, int y);

public:
  PupilXr(Pupil* pupil);
  ~PupilXr();

  // Needs the pupil's GL context current.
  bool Init();
  // False when no session runs; then the caller deletes this.
  bool RequestExit();

  bool Mirroring() const;
  void DrawMirror(int w, int h);

  static void IdleCallback(void* pxr);
};

} // endnamespace gled

#endif
