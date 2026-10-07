// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ZGlShader.h"

#include <Glasses/ZQueen.h>

#include <GL/glew.h>

#include <fstream>

using namespace gled;

#include "ZGlShader.c7"

// ZGlShader

//______________________________________________________________________________
//
//

//==============================================================================

void ZGlShader::_init()
{
  mType = 0;
  bAutoRecompile = true;

  bCompiled = false;
}

ZGlShader::ZGlShader(const Text_t* n, const Text_t* t) :
  ZGlass(n, t)
{
  _init();
}

ZGlShader::~ZGlShader()
{}

//==============================================================================

void ZGlShader::SetProgram(const Text_t* s)
{
  mProgram = s;
  Stamp(FID());
  if (bAutoRecompile)
    EmitRecompileRay();
}

void ZGlShader::EmitRecompileRay()
{
  if (mQueen && mSaturn->AcceptsRays()) {
    std::unique_ptr<Ray> ray
      (Ray::PtrCtor(this, PRQN_recompile, mTimeStamp, FID()));
    mQueen->EmitRay(ray);
  }
}

//==============================================================================

void ZGlShader::Load(const Text_t* file)
{
  if (file != 0)
  {
    mFile = file;
    if (mFile.EndsWith(".vert"))
      mType = GL_VERTEX_SHADER;
    else if (mFile.EndsWith(".frag"))
      mType = GL_FRAGMENT_SHADER;
    else
      mType = 0;
  }

  {
    GMutexHolder prog_lock(mProgMutex);
    std::ifstream f(mFile);
    mProgram.ReadFile(f);
    f.close();
  }

  Stamp(FID());
}

void ZGlShader::Save(const Text_t* file)
{
  std::ofstream f(mFile);
  f << mProgram;
  f.close();
}

//==============================================================================

TString ZGlShader::type_as_string()
{
  switch (mType)
  {
    case GL_VERTEX_SHADER:   return "Vertex";
    case GL_FRAGMENT_SHADER: return "Fragment";
    default:                 return "Undef";
  }
}

void ZGlShader::PrintProgram()
{
  std::cout << type_as_string() << " program for " << Identify() << "\n" << mProgram;
}

void ZGlShader::PrintLog()
{
  std::cout << "Compile log for " << Identify() << "\n" << mLog;
}
