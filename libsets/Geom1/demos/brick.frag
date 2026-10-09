// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

// Procedural bricks in a running bond, in object coordinates, each with its
// own shade; lit by GL light 0.

#version 120

uniform vec3  BrickColor  = vec3(0.66, 0.27, 0.17);
uniform vec3  MortarColor = vec3(0.78, 0.76, 0.72);
uniform vec2  BrickSize   = vec2(0.40, 0.14);  // length and height
uniform float MortarWidth = 0.025;
uniform float ShadeSpread = 0.25;              // variation among bricks

varying vec3 ObjPos;
varying vec3 ObjNormal;
varying vec3 EyePos;
varying vec3 EyeNormal;

float hash(vec2 p)
{
  return fract(sin(dot(p, vec2(12.9898, 78.233))) * 43758.5453);
}

void main()
{
  vec3 an = abs(ObjNormal);
  vec2 q;
  if (an.z > an.x && an.z > an.y) q = ObjPos.xy;
  else if (an.x > an.y)           q = ObjPos.yz;
  else                            q = ObjPos.xz;

  vec2  p   = q / BrickSize;
  float row = floor(p.y);
  p.x += 0.5 * mod(row, 2.0);

  vec2 cell = floor(p);
  vec2 f    = fract(p) * BrickSize;               // inside the brick, object units
  vec2 h    = vec2(0.5 * MortarWidth);
  vec2 aa   = fwidth(q);                          // soften the mortar edges
  vec2 in2  = smoothstep(h - aa, h + aa, f) * (1.0 - smoothstep(BrickSize - h - aa, BrickSize - h + aa, f));
  float brick = in2.x * in2.y;

  vec3 shade = BrickColor * (1.0 - ShadeSpread * 0.5 + ShadeSpread * hash(cell));
  vec3 color = mix(MortarColor, shade, brick);

  vec3  n = normalize(EyeNormal);
  vec4  lp = gl_LightSource[0].position;
  vec3  l = normalize(lp.w == 0.0 ? lp.xyz : lp.xyz - EyePos);
  float diffuse  = max(dot(n, l), 0.0);
  float specular = pow(max(dot(reflect(-l, n), normalize(-EyePos)), 0.0), 16.0);

  gl_FragColor = vec4(color * (0.25 + 0.75 * diffuse) + 0.12 * brick * specular, 1.0);
}
