// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef GledCore_PupilXr_H
#define GledCore_PupilXr_H

namespace gled {

class Pupil;

// An OpenXR session that renders a pupil's scene into a headset, from an
// FLTK idle callback. Started by Pupil when GLED_XR is set.
class PupilXr
{
  struct Impl;
  Impl  *m;

  void frame();
  bool poll_events();
  void render_eye(int eye);

public:
  PupilXr(Pupil* pupil);
  ~PupilXr();

  // Needs the pupil's GL context current.
  bool Init();

  static void IdleCallback(void* pxr);
};

} // endnamespace gled

#endif
