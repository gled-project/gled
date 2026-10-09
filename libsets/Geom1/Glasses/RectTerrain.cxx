// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

//__________________________________________________________________________
//
// A rectangular mNx * mNy height field.
// mO(x|y) ~ origin, mD(x|y) ~ grid spacing.
// Can be set from a ZImage via void SetFromImage(ZImage* image), from a TH2
// via SetFromHisto(TH2* histo) or from an array of heights via SetFromArray().
//
// If ColSep == 0, coloring is not done.
//
// Renderer is stupid, so expect it to be slow for large grids (512x512).

#include "RectTerrain.h"
#include <Glasses/ZImage.h>
#include <Stones/TringTvor.h>
#include <Gled/TRootXTReq.h>

#include <IL/il.h>

#include <TMath.h>
#include <TRandom.h>
#include <TCanvas.h>
#include <TROOT.h>

#include <cstdio>

using namespace gled;

#include "RectTerrain.c7"

Float_t RectTerrain::sMaxEpsilon = 0.0001;

/**************************************************************************/

void RectTerrain::_init()
{
  // Override settings from ZGlass
  bUseDispList = true;

  mNx = mNy = 0;
  mDx = mDy = 1;

  mMinZ = 0; mMaxZ = 0;
  mMinCol.gray(0.2); mMaxCol.gray(1);
  mColSep = 1;

  mRnrMode    = RM_Histo;
  mBorderCond = BC_Zero;
  mOriginMode = OM_Edge;
  mBValue = 0;
  bBorder = false;

  mSmoothFac = 0.2;

  bStudySize = false;

  pTTvor      = 0;
  mTTvorStamp = 0;
  bUseTringStrips    = false;
  mMaxTSVerts        = 256;
  bRndColTringStrips = false;
}

RectTerrain::~RectTerrain()
{
  delete pTTvor;
}

/**************************************************************************/

void RectTerrain::ApplyBorderCondition()
{
  const Int_t MX = mNx + 1;
  const Int_t MY = mNy + 1;
  const Int_t mx = mNx - 1;
  const Int_t my = mNy - 1;

  switch (mBorderCond) {

  case BC_Zero:
  case BC_Const: {
    const Float_t C = (mBorderCond == BC_Zero) ? 0 : mBValue;
    mP(0,0) = mP(MX,MY) = C;
    for(Int_t i=0; i<=MX; ++i) { mP(i, 0) = mP(i, MY) = C; }
    for(Int_t i=1; i< MY; ++i) { mP(0, i) = mP(MX, i) = C; }
    break;
  }
  case BC_External: {
    break;
  }
  case BC_Equal: {
    mP(0 ,0) = mP(1,1);   mP( 0,MY) = mP(1,mNy);
    mP(MX,0) = mP(mNx,1); mP(MX,MY) = mP(mNx,mNy);
    for(Int_t i=1; i<MX; ++i) { mP(i, 0) = mP(i,1); mP(i, MY) = mP(i,mNy); }
    for(Int_t i=1; i<MY; ++i) { mP(0, i) = mP(1,i); mP(MX, i) = mP(mNx,i); }
    break;
  }
  case BC_Sym: {
    mP(0 ,0) = mP(2,2);  mP( 0,MY) = mP(2,my);
    mP(MX,0) = mP(mx,2); mP(MX,MY) = mP(mx,my);
    for(Int_t i=1; i<MX; ++i) { mP(i, 0) = mP(i,2); mP(i, MY) = mP(i,my); }
    for(Int_t i=1; i<MY; ++i) { mP(0, i) = mP(2,i); mP(MX, i) = mP(mx,i); }
    break;
  }
  case BC_ASym: {
    mP(0 ,0) = 2*mP(1,1)   - mP(2,2);  mP( 0,MY) = 2*mP(1,  mNy) - mP(2,my);
    mP(MX,0) = 2*mP(mNx,1) - mP(mx,2); mP(MX,MY) = 2*mP(mNx,mNy) - mP(mx,my);
    for(Int_t i=1; i<MX; ++i) {
      mP(i, 0) = 2*mP(i,1) - mP(i,2); mP(i, MY) = 2*mP(i,mNy) - mP(i,my);
    }
    for(Int_t i=1; i<MY; ++i) {
      mP(0, i) = 2*mP(1,i) - mP(2,i); mP(MX, i) = 2*mP(mNx,i) - mP(mx,i);
    }
    break;
  }
  case BC_Wrap: {
    mP(0, 0) = mP(mNx,mNy); mP(0, MY) = mP(1,mNy);
    mP(MX,0) = mP(1,  mNy); mP(MX,MY) = mP(1,1);
    for(Int_t i=1; i<MX; ++i) { mP(i, 0) = mP(i,mNy); mP(i, MY) = mP(i,1); }
    for(Int_t i=1; i<MY; ++i) { mP(0, i) = mP(mNx,i); mP(MX, i) = mP(1,i); }
    break;
  }

  } // end switch mBCond

  mStampReqTring = Stamp(FID());
}

/**************************************************************************/

void RectTerrain::SetFromImage(ZImage* image, Float_t zfac)
{
  static const Exc_t _eh("RectTerrain::SetFromImage ");

  if(!image->IsBindable())
    throw(_eh + "image '" + image->Identify() + "' not bindable.");

  ZImage::sILMutex.Lock();
  image->bind();
  int w = image->w(), h = image->h();
  ILushort* data = new ILushort[w*h];
  ilCopyPixels(0,0,0, w,h,1, IL_LUMINANCE,IL_UNSIGNED_SHORT, data);
  image->unbind();
  ZImage::sILMutex.Unlock();

  mNx = w; mNy = h;
  mP.Clear();
  mP.ResizeTo(mNx + 2, mNy + 2);
  mMinZ = 1e9; mMaxZ = -1e9;

  for(Int_t y=1; y<=mNy; ++y) {
    ILushort* bar = &(data[(y-1)*w]);
    for(Int_t x=1; x<=mNx; ++x) {
      const float z = (float)(*(bar++));
      if(z > mMaxZ) mMaxZ = z;
      if(z < mMinZ) mMinZ = z;
      mP(x,y) = z;
    }
  }
  delete [] data;

  float zoffset = 0; // set z = 0 at 0

  float delta = (1 + sMaxEpsilon)*(mMaxZ - mMinZ);
  for(Int_t x=1; x<=mNx; ++x) {
    for(Int_t y=1; y<=mNy; ++y) {
      mP(x,y) = ( (mP(x,y) - mMinZ)/delta - zoffset ) * zfac;
    }
  }

  if(zfac > 0) {
    mMinZ = -zoffset * zfac; mMaxZ = (1 - zoffset) * zfac;
  } else {
    mMaxZ = -zoffset * zfac; mMinZ = (1 - zoffset) * zfac;
  }

  mStampReqTring = Stamp(FID());
}

void RectTerrain::SetFromHisto(TH2* histo)
{
  mNx = histo->GetNbinsX(); mNy = histo->GetNbinsY();
  const Int_t MX = mNx + 1, MY = mNy + 1;
  mP.Clear();
  mP.ResizeTo(mNx+2, mNy+2);
  mMinZ = 1e9; mMaxZ = -1e9;

  for(Int_t x=0; x<=MX; ++x) {
    for(Int_t y=0; y<=MY; ++y) {
      mP(x,y) = histo->GetBinContent(x,y);
    }
  }
  mMinZ = histo->GetMinimum();
  mMaxZ = histo->GetMaximum();

  float zoffset = 0; // set z = 0 at 0
  float zfac = 1;    // scale not

  float delta = 1.001*(mMaxZ - mMinZ);
  for(Int_t x=0; x<=MX; ++x) {
    for(Int_t y=0; y<=MY; ++y) {
      mP(x,y) = ( (mP(x,y) - mMinZ)/delta - zoffset ) * zfac;
    }
  }

  if(zfac > 0) {
    mMinZ = -zoffset * zfac; mMaxZ = (1 - zoffset) * zfac;
  } else {
    mMaxZ = -zoffset * zfac; mMinZ = (1 - zoffset) * zfac;
  }

  mStampReqTring = Stamp(FID());
}

void RectTerrain::SetFromArray(Int_t nx, Int_t ny, const Float_t* data, Float_t zfac)
{
  // Takes nx * ny heights from data, row by row, multiplied by zfac. Then
  // applies the border condition and recalculates the height range.

  mNx = nx; mNy = ny;
  mP.Clear();
  mP.ResizeTo(mNx + 2, mNy + 2);

  for(Int_t y=1; y<=mNy; ++y) {
    const Float_t* bar = &data[(y-1)*nx];
    for(Int_t x=1; x<=mNx; ++x) {
      mP(x,y) = zfac * (*(bar++));
    }
  }

  ApplyBorderCondition();
  RecalcMinMax();
}

/**************************************************************************/

void RectTerrain::Smooth(Float_t fac)
{
  TMatrixF M(mP);
  Double_t fa = (fac == 0) ? mSmoothFac : fac;
  Double_t fb = fa/TMath::Sqrt(2.0);
  Double_t normfac = 1/(1 + 4*fa + 4*fb);
  for(Int_t i=1; i<=mNx; ++i) {
    for(Int_t j=1; j<=mNy; ++j) {
      mP(i,j) = (M(i,j) + fa*(M(i-1,j) + M(i+1,j) + M(i,j-1) + M(i,j+1)) +
                      fb*(M(i-1,j-1) + M(i+1,j+1) + M(i+1,j-1) + M(i-1,j+1))
		 ) * normfac;
    }
  }
  mStampReqTring = Stamp(FID());
}

void RectTerrain::RecalcMinMax()
{
  mMinZ = 1e9; mMaxZ = -1e9;
  for(Int_t i=1; i<=mNx; ++i) {
    for(Int_t j=1; j<=mNy; ++j) {
      if(mP(i,j) > mMaxZ) mMaxZ = mP(i,j);
      if(mP(i,j) < mMinZ) mMinZ = mP(i,j);
    }
  }
  mMaxZ *= (1 + sMaxEpsilon);
  mStampReqTring = Stamp(FID());
}

/**************************************************************************/

void RectTerrain::ReTring()
{
  mStampReqTring = Stamp(FID());
}

void RectTerrain::Edenify()
{
  // Demo of SetFromHisto() and Smooth(). Runs detached and locks only to set
  // the terrain; the canvas is drawn by a request it does not wait for.

  TH2F* h = new TH2F("Eden", "GardenOfEdenify", 128, -5, 5, 128, -5, 5);
  h->SetDirectory(0);
  TRandom r;
  const Double_t sig = 1.4;
  for(int i=0;i<400000;++i) h->Fill(r.Uniform(-5, 5), r.Uniform(-5, 5));
  for(int i=0;i<700000;++i) h->Fill(sig*r.Gaus(), sig*r.Gaus());

  // The rivers are filled with negative weights that carve a constant depth,
  // as deep as the mount is high. Smoothing makes the narrow rivers
  // shallower, and the plain lands at about 0.4 of the height range.
  const Double_t bin_area = (10.0/128)*(10.0/128);
  const Double_t mount    = 700000 / (TMath::TwoPi()*sig*sig) * bin_area;
  const Int_t    n_river  = 60000;
  const Double_t width    = 0.18;
  const Double_t density  = n_river / (6.9 * width * 2.5) * bin_area;
  const Double_t depth    = mount;
  for(int k=0; k<4; ++k) {
    const Double_t phi = k * TMath::PiOver2() + 0.4;
    const Double_t c = TMath::Cos(phi), s = TMath::Sin(phi);
    for(int i=0; i<n_river; ++i) {
      const Double_t t = r.Uniform(0.6, 7.5);
      const Double_t d = 0.35*TMath::Sin(1.7*t + k) + width*r.Gaus();
      const Double_t x = t*c - d*s, y = t*s + d*c;
      if (TMath::Abs(x) >= 5 || TMath::Abs(y) >= 5)
        continue; // keep negative weights out of the overflow bins
      h->Fill(x, y, -depth / density);
    }
  }
  TH2* show = (TH2*) h->Rebin2D(2, 2, "EdenShow");
  show->SetDirectory(0);
  show->SetBit(kCanDelete);
  TRootXTReq::Post([show]()
  {
    TCanvas *c = (TCanvas*) gROOT->GetListOfCanvases()->FindObject("Eden");
    if (c == 0)
      c = new TCanvas("Eden", "GardenOfEdenify");
    c->cd();
    c->Clear();
    show->Draw("SURF1");
    c->Modified();
    c->Update();
  });

  {
    GLensWriteHolder wlck(this);
    SetFromHisto(h);
    for(int i=0; i<5; ++i) Smooth(1);
    RecalcMinMax();
  }
  delete h;
}

/**************************************************************************/

void RectTerrain::color_filler(Float_t* v, UChar_t* c, void* rt)
{
  ((RectTerrain*)rt)->make_color(v[2]).to_ubyte(c);
}

TringTvor* RectTerrain::SpawnTringTvor(Bool_t smoothp, Bool_t flatp,
                                       Bool_t colp,    Bool_t texp)
{
  // Create TringTvor from height field.
  // This can be also be called by an outside client that wants to extract
  // triangulation of this terrain as a triangle mesh.

  Int_t minX = 1, maxX = mNx, minY = 1, maxY = mNy;
  if(bBorder) {
    --minX; ++maxX; --minY; ++maxY;
  }
  Int_t       nx = maxX - minX + 1,       ny = maxY - minY + 1;
  Float_t tex_fx = 1.0/(maxX - minX), tex_fy = 1.0/(maxY - minY);

  TringTvor& TT = * new TringTvor(nx*ny, (nx-1)*(ny-1)*2, smoothp, colp, texp);

  Int_t          idx = 0;
  Int_t    tring_idx = 0;

  for(Int_t j=minY; j<=maxY; ++j)
  {
    for(Int_t i=minX; i<=maxX; ++i)
    {
      Float_t *v = TT.Vertex(idx);
      v[0] = (i-1)*mDx; v[1] = (j-1)*mDy; v[2] = mP[i][j];

      if (smoothp)
      {
	Int_t il=i,ih=i,jh=j,jl=j;
	if(i>0) il--;
	if(i<=mNx) ih++;
	if(j>0) jl--;
	if(j<=mNy) jh++;
	Float_t dvx[] = { (ih-il)*mDx, 0, mP[ih][j] - mP[il][j] };
	Float_t dvy[] = { 0, (jh-jl)*mDy, mP[i][jh] - mP[i][jl] };
	Float_t *n = TT.Normal(idx);
	n[0] = -dvy[1]*dvx[2];
        n[1] = -dvx[0]*dvy[2];
        n[2] =  dvx[0]*dvy[1];
        TMath::Normalize(n);

	if (colp)
	{
	  make_color(mP[i][j]).to_ubyte(TT.Color(idx));
	}
      }

      if (texp)
      {
	Float_t* t = TT.Texture(idx);
	t[0] = tex_fx*(i-minX);
	t[1] = tex_fy*(j-minY);
      }

      if (i < maxX && j < maxY)
      {
	Float_t d1 = TMath::Abs(mP[i][j]   - mP[i+1][j+1] );
	Float_t d2 = TMath::Abs(mP[i][j+1] - mP[i+1][j]   );
	Int_t* T = TT.Triangle(tring_idx);
	// printf("j=%3d i=%3d idx=%4d tidx=%4d d1=%f, d2=%f\n",
        //        j, i, idx, tring_idx, d1, d2);
	if(d1 > d2) {
	  T[0] = idx;   T[1] = idx+1;    T[2] = idx+nx;   T += 3;
	  T[0] = idx+1; T[1] = idx+nx+1; T[2] = idx+nx;
	} else {
	  T[0] = idx;   T[1] = idx+1;    T[2] = idx+nx+1; T += 3;
	  T[0] = idx;   T[1] = idx+nx+1; T[2] = idx+nx;
	}
	tring_idx += 2;
      }

      ++idx;
    }
  }

  if(flatp) {
    if(colp)
      TT.GenerateTriangleNormalsAndColors(color_filler, this);
    else
      TT.GenerateTriangleNormals();
  }

  return &TT;
}

void RectTerrain::MakeTringTvor()
{
  // Creates TringTvor appropriate for current data-member settings.

  Bool_t smoothp = (mRnrMode == RM_SmoothTring);
  Bool_t flatp   = ! smoothp;
  Bool_t colp    = (mColSep  != 0);
  Bool_t texp    = (mTexture != 0);

  delete pTTvor;
  pTTvor = SpawnTringTvor(smoothp, flatp, colp, texp);

  if(bUseTringStrips) {
    pTTvor->GenerateTriangleStrips(mMaxTSVerts);
    if(bRndColTringStrips) {
      pTTvor->GenerateTriangleColorsFromTriangleStrips();
    }
  }

  mTTvorStamp = mTimeStamp;
}

/**************************************************************************/

Float_t RectTerrain::GetMinX(Bool_t include_border) const
{
  return (bBorder && include_border) ? -mDx : 0;
}

Float_t RectTerrain::GetMaxX(Bool_t include_border) const
{
  Int_t n = (bBorder && include_border) ? mNx : mNx-1;
  return n*mDx;
}

Float_t RectTerrain::GetMinY(Bool_t include_border) const
{
  return (bBorder && include_border) ? -mDy : 0;
}

Float_t RectTerrain::GetMaxY(Bool_t include_border) const
{
  Int_t n = (bBorder && include_border) ? mNy : mNy-1;
  return n*mDy;
}

