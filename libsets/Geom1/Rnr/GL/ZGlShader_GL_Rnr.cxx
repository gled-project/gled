// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "ZGlShader_GL_Rnr.h"
#include <Stones/ZMIR.h>
#include <Eye/Eye.h>

#include <GL/glew.h>

#include <vector>

using namespace gled;


#define PARENT ZGlass_GL_Rnr

//______________________________________________________________________________
//
// This class is never rendered/drawn. Functions 
//   GLuint AssertShader() and
//   GLuint Compile()
// are called from ZGlProgram_GL_Rnr when needed.
//
// Need compilation failed flag - so that we don't retry this in render loop.
// Clear this on recompile ray.

//==============================================================================

void ZGlShader_GL_Rnr::_init()
{
  bRecompile = false;
  mShaderID = 0;
}

ZGlShader_GL_Rnr::ZGlShader_GL_Rnr(ZGlShader* idol) :
  ZGlass_GL_Rnr(idol),
  mZGlShader(idol)
{
  _init();
}

ZGlShader_GL_Rnr::~ZGlShader_GL_Rnr()
{}

//==============================================================================

void ZGlShader_GL_Rnr::AbsorbRay(Ray& ray)
{
  // Only handle ZGlShader::PRQN_recompile.

  using namespace RayNS;

  if(ray.fFID == ZGlShader::FID())
  {
    switch (ray.fRQN)
    {
      case ZGlShader::PRQN_recompile:
        std::cout << "Recompile " << mZGlShader->Identify() << std::endl;
        bRecompile = true;
        return;
      default:
        break;
    }
  }
  PARENT::AbsorbRay(ray);
}

//==============================================================================

GLuint ZGlShader_GL_Rnr::AssertShader()
{
  if (mShaderID && !bRecompile)
    return mShaderID;
  else
    return Compile();
}

GLuint ZGlShader_GL_Rnr::Compile()
{
  bRecompile = false;

  if (mShaderID == 0)
  {
    mShaderID = glCreateShader(mZGlShader->mType);
    if (mShaderID == 0)
    {
      std::cout << "Failed creating shader " << mZGlShader->Identify() << std::endl;
      return 0;
    }
  }

  {
    GMutexHolder prog_lock(mZGlShader->mProgMutex);
    const char* program =  mZGlShader->mProgram.Data();
    glShaderSource(mShaderID, 1, &program, 0);
  }

  glCompileShader(mShaderID);

  GLint status;
  glGetShaderiv(mShaderID, GL_COMPILE_STATUS, &status);
  std::unique_ptr<ZMIR> s_mir(mZGlShader->S_SetCompiled(status == GL_TRUE));
  fImg->fEye->Send(*s_mir);

  GLint log_len;
  glGetShaderiv(mShaderID, GL_INFO_LOG_LENGTH, &log_len);
  std::vector<GLchar> log(log_len > 0 ? log_len : 1, 0);
  if (log_len > 0)
    glGetShaderInfoLog(mShaderID, log_len, 0, log.data());
  std::unique_ptr<ZMIR> l_mir(mZGlShader->S_SetLog(log.data()));
  fImg->fEye->Send(*l_mir);

  std::cout << "Compilation log for " << mZGlShader->Identify() << "\n" << log.data();

  if (status == GL_FALSE)
  {
    glDeleteShader(mShaderID);
    mShaderID = 0;
  }

  return mShaderID;
}
