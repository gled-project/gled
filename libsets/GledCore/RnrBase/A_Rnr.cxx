// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "A_Rnr.h"
#include "RnrDriver.h"

#include <Eye/Ray.h>

#include <algorithm>

using namespace gled;


namespace GNS  = GledNS;
namespace GVNS = GledViewNS;
namespace OS = OptoStructs;

const int A_Rnr::sMaxRnrLevel = 7;

/**************************************************************************/

A_Rnr::~A_Rnr()
{
  delete mProgram;
}

void A_Rnr::AbsorbRay(Ray& ray)
{
  if (ray.fEyeBits & Ray::EB_StructuralChange)
    MarkProgramDirty();
}

void A_Rnr::MarkProgramDirty()
{
  if (mProgram) mProgram->bDirty = true;
}

bool A_Rnr::HasOutsideEffects()
{
  // Draw-only classes are rnr-mods; as list elements they set state with no restore.
  const GNS::RnrBits& rb = fImg->GetCI()->fDefRnrCtrl.fRnrBits;
  return rb.fSelf[1] && ! rb.fSelf[0] && ! rb.fSelf[2];
}

/**************************************************************************/

void A_Rnr::crs_links(RnrDriver* rd, RnrScheme* sch)
{
  // Fill links and links as lists.
  OS::lZLinkDatum_i ldatum = fImg->fLinkData.begin();
  OS::lZLinkDatum_i ldend  = fImg->fLinkData.end();
  for (; ldatum != ldend; ++ldatum)
  {
    if (ldatum->fToGlass == 0) continue;
    OS::ZGlassImg* limg = ldatum->GetToImg();
    const GNS::RnrBits& rb = ldatum->GetLinkInfo()->fDefRnrBits;

    if (rb.SelfOn())
    {
      rd->FillRnrScheme(sch, rd->GetRnr(limg), rb);
    }

    if (rb.ListOn() && limg->IsList())
    {
      sch->AddDep(limg);
      rd->FillRnrScheme(sch, limg->GetElementImgs(), rb);
    }
  }
}

void A_Rnr::crs_self(RnrDriver* rd, RnrScheme* sch)
{
  // Fill self.
  GNS::RnrBits& rb(fImg->GetCI()->fDefRnrCtrl.fRnrBits);
  if (rb.SelfOn())
  {
    rd->FillRnrScheme(sch, this, rb);
  }
}

void A_Rnr::crs_elements(RnrDriver* rd, RnrScheme* sch)
{
  // Fill self as list.
  GNS::RnrBits& rb(fImg->GetCI()->fDefRnrCtrl.fRnrBits);
  if (rb.ListOn() && fImg->IsList())
  {
    rd->FillRnrScheme(sch, fImg->GetElementImgs(), rb);
  }
}

void A_Rnr::CreateRnrScheme(RnrDriver* rd, RnrScheme* sch)
{
  crs_links(rd, sch);
  crs_self(rd, sch);
  crs_elements(rd, sch);
}

/**************************************************************************/
// A_Rnr::Fl_Event

namespace
{
  const char* fl_event_names[] =
  {
    "NO_EVENT",
    "PUSH",
    "RELEASE",
    "ENTER",
    "LEAVE",
    "DRAG",
    "FOCUS",
    "UNFOCUS",
    "KEYDOWN",
    "KEYUP",
    "CLOSE",
    "MOVE",
    "SHORTCUT",
    "DEACTIVATE",
    "ACTIVATE",
    "HIDE",
    "SHOW",
    "PASTE",
    "SELECTIONCLEAR",
    "MOUSEWHEEL",
    "DND_ENTER",
    "DND_DRAG",
    "DND_LEAVE",
    "DND_RELEASE"
  };
}

void A_Rnr::Fl_Event::dump()
{
  printf("%s: key=%d but=%d state=0x%x text='%s' len=%d\n",
	 fl_event_names[fEvent], fKey, fButton, fState,
	 fText.Data(), fText.Length());
}

/**************************************************************************/
// RnrScheme
/**************************************************************************/

void RnrScheme::AddDep(OS::ZGlassImg* img)
{
  if (std::find(fDeps.begin(), fDeps.end(), img) == fDeps.end())
    fDeps.push_back(img);
}

void RnrScheme::Flatten(vRnrOp_t& ops) const
{
  ops.clear();
  for (int l = 1; l <= A_Rnr::sMaxRnrLevel; ++l)
    ops.insert(ops.end(), fLevels[l].begin(), fLevels[l].end());
}

/**************************************************************************/
// RnrProgram
/**************************************************************************/

RnrProgram::DepObserver::~DepObserver()
{
  // The image is dying and takes its views along.
  if (fProg)
  {
    fProg->mDeps.erase(std::find(fProg->mDeps.begin(), fProg->mDeps.end(), this));
    fProg->bDirty = true;
  }
}

void RnrProgram::DepObserver::AbsorbRay(Ray& ray)
{
  if (fProg && (ray.fEyeBits & Ray::EB_StructuralChange))
    fProg->bDirty = true;
}

RnrProgram::~RnrProgram()
{
  for (auto d : mDeps)
  {
    d->fProg = 0;
    delete d;
  }
}

void RnrProgram::Set(RnrDriver* rd, const RnrScheme& sch)
{
  sch.Flatten(fOps);

  for (auto i = mDeps.begin(); i != mDeps.end(); )
  {
    if (std::find(sch.fDeps.begin(), sch.fDeps.end(), (*i)->fImg) == sch.fDeps.end())
    {
      (*i)->fProg = 0;
      rd->RetireView(*i);
      i = mDeps.erase(i);
    }
    else ++i;
  }
  for (auto img : sch.fDeps)
  {
    if (std::find_if(mDeps.begin(), mDeps.end(),
                     [img](DepObserver* d) { return d->fImg == img; }) == mDeps.end())
      mDeps.push_back(new DepObserver(img, this));
  }

  bDirty = false;
}

bool RnrProgram::Matches(const RnrScheme& sch) const
{
  vRnrOp_t ops;
  sch.Flatten(ops);
  if (ops != fOps || sch.fDeps.size() != mDeps.size())
    return false;
  for (auto img : sch.fDeps)
  {
    if (std::find_if(mDeps.begin(), mDeps.end(),
                     [img](DepObserver* d) { return d->fImg == img; }) == mDeps.end())
      return false;
  }
  return true;
}
