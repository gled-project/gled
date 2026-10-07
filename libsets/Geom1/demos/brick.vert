// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

// Procedural bricks, vertex shader: passes the object-space position and
// normal, for the brick pattern, and the eye-space ones, for the lighting.

#version 120

varying vec3 ObjPos;
varying vec3 ObjNormal;
varying vec3 EyePos;
varying vec3 EyeNormal;

void main()
{
  ObjPos      = gl_Vertex.xyz;
  ObjNormal   = gl_Normal;
  EyePos      = vec3(gl_ModelViewMatrix * gl_Vertex);
  EyeNormal   = gl_NormalMatrix * gl_Normal;
  gl_Position = ftransform();
}
