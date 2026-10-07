// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

//__________________________________________________________________________
// ZGeoRepacker
//
//

#include "ZGeoRepacker.h"
using namespace gled;
#include "ZGeoRepacker.c7"

#include <Stones/TGeoShapeExtract.h>

#include <TGeoManager.h>

#include <TFile.h>

/**************************************************************************/

void ZGeoRepacker::_init()
{
  // *** Set all links to 0 ***
  mRoot = 0;
  mOutFile = "simple_geo.root";
}

/**************************************************************************/

void ZGeoRepacker::RepackGeometry()
{
  mSrcGeo = gGeoManager; // Perhaps should import it independently.

  gGeoManager = 0;
  mNeoGeo = new TGeoManager("MicroALICE", "Very Simplified ALICE geometry");
  gGeoManager = mSrcGeo;

  repack_geometry(*mRoot, 0);

  gGeoManager = mNeoGeo;
  mNeoGeo->CloseGeometry();
  mNeoGeo->Export(mOutFile.Data(), "Xilpmis");
  delete mNeoGeo;
  mNeoGeo = 0;

  gGeoManager = mSrcGeo;
}

void ZGeoRepacker::repack_geometry(ZGeoNode* zgnode, TGeoVolume* parent_volume)
{
  bool recurse_p = false;
  TGeoNode*   tnode   = 0;
  TGeoVolume* tvolume = 0;
  TGeoShape*  tshape  = 0;

  tnode = zgnode->GetTNode();
  if(tnode == 0) {
    printf("Null node for %s; assuming it's a holder and descending.\n",
	   zgnode->Identify().Data());

    recurse_p = true;
    tvolume = parent_volume;
    goto recurse_loop;
  }

  tvolume = tnode->GetVolume();
  if(tvolume == 0) {
    printf("Null volume for %s; skipping.\n", zgnode->Identify().Data());
    return;
  }

  tshape  = tvolume->GetShape();
  if(tshape && HasShape(tshape) == false) {
    mNeoGeo->AddShape(tshape);
    PutShape(tshape);
  }
  // Materials, matrices?

  if(HasVolume(tvolume) == false) {
    tvolume->ClearNodes();
    tvolume->SetNtotal(0);
    mNeoGeo->AddVolume(tvolume);
    recurse_p = true;
  }
  PutVolume(tvolume);
  if(parent_volume == 0) {
    mNeoGeo->SetTopVolume(tvolume);
  } else {
    parent_volume->AddNode(tvolume, GetVolume(tvolume), tnode->GetMatrix());
  }

 recurse_loop:

  if(recurse_p) {
    std::list<ZGeoNode*> l; zgnode->CopyListByGlass<ZGeoNode>(l);
    for(std::list<ZGeoNode*>::iterator i=l.begin(); i!=l.end(); ++i)
      repack_geometry(*i, tvolume);
  }
}

/**************************************************************************/

void ZGeoRepacker::DumpShapeTree(const Text_t* name)
{
  TGeoShapeExtract* gse = dump_shape_tree(*mRoot, 0);
  TFile f(mOutFile, "UPDATE");
  gse->Write(name);
  f.Close();
}

TGeoShapeExtract*
ZGeoRepacker::dump_shape_tree(ZGeoNode* zgnode, TGeoShapeExtract* parent)
{
  TGeoNode*   tnode   = 0;
  TGeoVolume* tvolume = 0;
  TGeoShape*  tshape  = 0;

  tnode = zgnode->GetTNode();
  if(tnode == 0) {
    printf("Null node for %s; assuming it's a holder and descending.\n",
	   zgnode->Identify().Data());
    goto do_dump;
  }

  tvolume = tnode->GetVolume();
  if(tvolume == 0) {
    printf("Null volume for %s; skipping.\n", zgnode->Identify().Data());
    return 0;
  }

  tshape  = tvolume->GetShape();

do_dump:
  ZTrans trans;
  if (parent) trans.SetFromArray(parent->mTrans);
  trans *= zgnode->RefTrans();

  TGeoShapeExtract* gse = new TGeoShapeExtract(zgnode->GetName(),
					       zgnode->GetTitle());
  gse->SetTrans(trans.Array());
  gse->SetRGBA (zgnode->PtrColor()->array());
  gse->mRnrSelf     = zgnode->GetRnrSelf();
  gse->mRnrElements = zgnode->GetRnrElements();
  gse->mShape       = tshape;

  if ( ! zgnode->IsEmpty())
  {
    gse->mElements = new TList;
    gse->mElements->SetOwner(true);

    std::list<ZGeoNode*> l;
    zgnode->CopyListByGlass<ZGeoNode>(l);
    for(std::list<ZGeoNode*>::iterator i=l.begin(); i!=l.end(); ++i)
      dump_shape_tree(*i, gse);
  }

  if(parent)
    parent->mElements->Add(gse);

  return gse;
}
