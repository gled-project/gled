// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef Gled_SphereTrings
#define Gled_SphereTrings

#include <GL/glew.h>

namespace gled {

namespace SphereTrings {
  extern GLfloat  CubeA;
  extern GLfloat  OctusA;

  extern GLfloat *Vertexen[5];
  extern GLfloat *Normaleen[5];
  extern GLubyte *Indexen[5];
  extern GLsizei  IndexSize[5];
  extern GLenum   GLmode[5];

  void Render(int i, bool flat_p);

  void EnableGL(int i);
  void DrawAndDisableGL(int i);

  void UnitBox();
  void UnitFrameBox();

} // namespace SphereTrings

} // endnamespace gled

#endif
