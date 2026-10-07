// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "TringulaTester.h"
#include "Tringula.h"
#include "TSPupilInfo.h"

using namespace gled;

#include "TringulaTester.c7"

// TringulaTester

//______________________________________________________________________________
//
// Helper class to test functionality of Tringula and display the results.
//
// 1. Ray-terrain intersection
// 2. Detection of extendio intersection lines

//==============================================================================

void TringulaTester::_init()
{
  bRnrRay = false;
  mRayLen = 0;
  mRayPos.Zero();
  mRayDir.Zero();

  bRnrItsLines = false;
}

TringulaTester::TringulaTester(const Text_t* n, const Text_t* t) :
  ZNode(n, t)
{
  _init();
}

TringulaTester::~TringulaTester()
{}

//==============================================================================

void TringulaTester::SetRayVectors(const ZPoint& pos, const ZPoint& dir)
{
  mRayPos = pos;
  mRayDir = dir;
  Stamp(FID());
}

//==============================================================================

void TringulaTester::RayCollideTerrain()
{
  static const Exc_t _eh("TringulaTester::RayCollideTerrain ");

  assert_tringula(_eh);

  Opcode::Ray ray(&mRayPos[0], &mRayDir[0]);
  if (ray.mDir.NormalizeAndReport() == 0)
    throw _eh + "ray-direction vector is null.";

  Bool_t rnr_on = bRnrRay;
  {
    GLensReadHolder _lck(this);
    bRnrRay = false;
  }

  Bool_t status = mTringula->RayCollide(ray, mRayLen, false, false, mRayColFaces);

  printf("%s status = %d, n_faces = %d\n", _eh.Data(), status, mRayColFaces.GetNbFaces());
  for (UInt_t f=0; f<mRayColFaces.GetNbFaces(); ++f)
  {
    const Opcode::CollisionFace& cf = mRayColFaces.GetFaces()[f];
    printf("  %2d: t=%6d  d=%10f  u=%10f v=%10f\n",
           f, cf.mFaceID, cf.mDistance, cf.mU, cf.mV);
  }

  {
    GLensReadHolder _lck(this);
    bRnrRay = rnr_on;
    StampReqTring(FID());
  }
}

//==============================================================================

void TringulaTester::FullBoxPrunning(Bool_t accumulate, Bool_t verbose)
{
  Bool_t rnr_on = bRnrItsLines;
  {
    GLensReadHolder _lck(this);
    bRnrItsLines = false;
  }

  mTringula->DoFullBoxPrunning(mItsLines, accumulate, verbose);

  {
    GLensReadHolder _lck(this);
    bRnrItsLines = rnr_on;
    StampReqTring(FID());
  }
}
