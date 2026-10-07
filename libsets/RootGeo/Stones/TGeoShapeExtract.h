// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef RootGeo_TGeoShapeExtract_H
#define RootGeo_TGeoShapeExtract_H

#include <TNamed.h>

class TList;
class TGeoShape;

namespace gled {

class TGeoShapeExtract : public TNamed
{
  friend class ZGeoRepacker;

  TGeoShapeExtract(const TGeoShapeExtract&);            // Not implemented
  TGeoShapeExtract& operator=(const TGeoShapeExtract&); // Not implemented

protected:
  Double_t    mTrans[16];
  Float_t     mRGBA[4];
  Bool_t      mRnrSelf;
  Bool_t      mRnrElements;
  TGeoShape*  mShape;
  TList*      mElements;

public:
  TGeoShapeExtract(const Text_t* n="TGeoShapeExtract", const Text_t* t=0);
  ~TGeoShapeExtract();

  Bool_t HasElements();
  void   AddElement(TGeoShapeExtract* gse);

  void SetTrans(const Double_t arr[16]);
  void SetRGBA (const Float_t  arr[4]);
  void SetRnrSelf(Bool_t r)     { mRnrSelf = r;     }
  void SetRnrElements(Bool_t r) { mRnrElements = r; }
  void SetShape(TGeoShape* s)   { mShape = s;       }
  void SetElements(TList* e)    { mElements = e;    }

  Double_t*  GetTrans()       { return mTrans; }
  Float_t*   GetRGBA()        { return mRGBA;  }
  Bool_t     GetRnrSelf()     { return mRnrSelf;     }
  Bool_t     GetRnrElements() { return mRnrElements; }
  TGeoShape* GetShape()       { return mShape;    }
  TList*     GetElements()    { return mElements; }

  ClassDef(TGeoShapeExtract, 1);
}; // endclass TGeoShapeExtract

} // endnamespace gled

#endif
