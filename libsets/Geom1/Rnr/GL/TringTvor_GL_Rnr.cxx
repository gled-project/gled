// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "TringTvor_GL_Rnr.h"
#include <GL/glew.h>

#include <Rnr/GL/ZGlass_GL_Rnr.h>
#include <Rnr/GL/SphereTrings.h>
#include <RnrBase/RnrDriver.h>

#include <cmath>

using namespace gled;


//______________________________________________________________________________
// TringTvor_GL_Rnr
//

typedef TringTvor TT;

TringTvor_GL_Rnr::TringTvor_GL_Rnr(TringTvor* tvor) :
  mTT(*tvor)
{}

void TringTvor_GL_Rnr::BeginSetup()
{
  // Clear internal state, prepare for new setup.

  mNMode = mCMode = mTMode = TT::M_None;
  mNArr  = 0;
  mCArr  = 0;
  mTArr  = 0;

  bTriangleStripsRequested = bTriangleStrips = false;
}

void TringTvor_GL_Rnr::SetupStandard(Bool_t color_p, Bool_t texture_p)
{
  // Set in accordance with default normal, color and texture modes in TringTvor.

  static const Exc_t _eh("TringTvor_GL_Rnr::SetupStandard ");

  SetNormalArray (mTT.mNormalMode);
  SetColorArray  (mTT.mColorMode);
  SetTextureArray(mTT.mTextureMode);
}

void TringTvor_GL_Rnr::SetupStandardSmooth(Bool_t color_p, Bool_t texture_p)
{
  // Set for smooth rendering -- only existing arrays from TringTvor are used.

  static const Exc_t _eh("TringTvor_GL_Rnr::SetupStandardSmooth ");

  if (mTT.HasNorms())
  {
    mNMode = TT::M_PerVertex;
    mNArr  = mTT.Norms();
  }
  else
  {
    throw _eh + "vertex normal array does not exist.";
  }

  if (color_p && mTT.HasCols())
  {
    mCMode = TT::M_PerVertex;
    mCArr  = mTT.Cols();
  }
  if (mTT.HasTexs())
  {
    mTMode = TT::M_PerVertex;
    mTArr  = mTT.Texs();
  }
}

void TringTvor_GL_Rnr::SetupStandardFlat(Bool_t color_p, Bool_t texture_p)
{
  // Set for flat rendering -- only existing arrays from TringTvor are used.

  static const Exc_t _eh("TringTvor_GL_Rnr::SetupStandardFlat ");

  if (mTT.HasTringNorms())
  {
    mNMode = TT::M_PerTriangle;
    mNArr  = mTT.TringNorms();
  }
  else
  {
    throw _eh + "triangle normal array does not exist.";
  }

  if (color_p && mTT.HasTringCols())
  {
    mCMode = TT::M_PerTriangle;
    mCArr  = mTT.TringCols();
  }
  if (mTT.HasTexs())
  {
    mTMode = TT::M_PerVertex;
    mTArr  = mTT.Texs();
  }
}

void TringTvor_GL_Rnr::SetNormalArray(TringTvor::Mode_e mode, Float_t* narr)
{
  static const Exc_t _eh("TringTvor_GL_Rnr::SetNormalArray ");

  if (mode == TT::M_PerVertex)
  {
    if (narr == 0)
    {
      if (! mTT.HasNorms())
	throw _eh + "No vertex normals.";
      narr = mTT.Norms();
    }
    mNMode = mode;
    mNArr  = narr;
  }
  else if (mode == TT::M_PerTriangle)
  {
    if (narr == 0)
    {
      if (! mTT.HasTringNorms())
	throw _eh + "No triangle normals.";
      narr = mTT.TringNorms();
    }
    mNMode = mode;
    mNArr  = narr;
  }
}

void TringTvor_GL_Rnr::SetColorArray(TringTvor::Mode_e mode, UChar_t* carr)
{
  if (carr == 0)
  {
    if      (mode == TT::M_PerVertex)   carr = mTT.Cols();
    else if (mode == TT::M_PerTriangle) carr = mTT.TringCols();
  }
  if (carr != 0)
  {
    mCMode = mode;
    mCArr  = carr;
  }
}

void TringTvor_GL_Rnr::SetTextureArray(TringTvor::Mode_e mode, Float_t* tarr)
{
  if (tarr == 0)
  {
    if      (mode == TT::M_PerVertex)   tarr = mTT.Texs();
    else if (mode == TT::M_PerTriangle) tarr = mTT.TringTexs();
  }
  if (tarr != 0)
  {
    mTMode = mode;
    mTArr  = tarr;
  }
}

void TringTvor_GL_Rnr::RequestTriangleStrips()
{
  bTriangleStripsRequested = true;
}

void TringTvor_GL_Rnr::EndSetup()
{
  // Check requested setup sanity, set rendering mode.

  static const Exc_t _eh("TringTvor_GL_Rnr::EndSetup ");

  // Check smooth / flat / mixed mode.
  if ((mNMode == TT::M_PerVertex) &&
      (mCMode == TT::M_PerVertex || mCMode == TT::M_None) &&
      (mTMode == TT::M_PerVertex || mTMode == TT::M_None))
  {
    mRenderMode = RM_Smooth;
    if (bTriangleStripsRequested && mTT.HasTrianlgeStrips())
      bTriangleStrips = true;
  }
  else if ((mNMode == TT::M_PerTriangle) &&
	   (mCMode == TT::M_PerTriangle || mCMode == TT::M_None) &&
	   (mTMode == TT::M_PerVertex   || mTMode == TT::M_None))
  {
    // Flat
    mRenderMode = RM_Flat;
    if (bTriangleStripsRequested && mTT.HasTrianlgeStrips())
      bTriangleStrips = true;
  }
  else
  {
    mRenderMode = RM_Mixed;
    if (bTriangleStripsRequested)
      throw _eh + "Triangle-strips requested -- not possible in mixed mode.";
  }
}

//==============================================================================

void TringTvor_GL_Rnr::BeginRender()
{
  glPushClientAttrib(GL_CLIENT_VERTEX_ARRAY_BIT);
  glVertexPointer(3, GL_FLOAT, 0, mTT.Verts());
  glEnableClientState(GL_VERTEX_ARRAY);

  if (mNMode == TT::M_PerVertex)
  {
    glNormalPointer(GL_FLOAT, 0, mNArr);
    glEnableClientState(GL_NORMAL_ARRAY);
  }
  if (mCMode == TT::M_PerVertex)
  {
    glColorPointer(4, GL_UNSIGNED_BYTE, 0, mCArr);
    glEnableClientState(GL_COLOR_ARRAY);
  }
  if (mTMode == TT::M_PerVertex)
  {
    glTexCoordPointer(2, GL_FLOAT, 0, mTArr);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  }
}

void TringTvor_GL_Rnr::Render()
{
  static const Exc_t _eh("TringTvor_GL_Rnr::Render ");

  switch (mRenderMode)
  {
    case RM_Smooth: RenderSmooth(); break;
    case RM_Flat:   RenderFlat();   break;
    case RM_Mixed:  RenderMixed();  break;
    default:        throw _eh + "Bad render mode.";
  }
}

void TringTvor_GL_Rnr::RenderSmooth()
{
  render_smooth(0, mTT.mNTrings, 0, mTT.mNStrips);
}

void TringTvor_GL_Rnr::RenderFlat()
{
  render_flat(0, mTT.mNTrings, 0, mTT.mNStrips);
}

void TringTvor_GL_Rnr::RenderMixed()
{
  render_mixed(0, mTT.mNTrings);
}

void TringTvor_GL_Rnr::render_smooth(Int_t t0, Int_t nt, Int_t s0, Int_t ns)
{
  if (bTriangleStrips)
  {
    glMultiDrawElements(GL_TRIANGLE_STRIP, mTT.mStripLens + s0, GL_UNSIGNED_INT,
			(const GLvoid**) (mTT.mStripBegs + s0), ns);
  }
  else
  {
    glDrawElements(GL_TRIANGLES,  3*nt,
		   GL_UNSIGNED_INT, mTT.Trings() + 3*t0);
  }
}

void TringTvor_GL_Rnr::render_flat(Int_t t0, Int_t nt, Int_t s0, Int_t ns)
{
  const Bool_t col_p = mCMode == TT::M_PerTriangle;

  if (bTriangleStrips)
  {
    for (Int_t i=s0; i<s0+ns; ++i)
    {
      glBegin(GL_TRIANGLE_STRIP);
      Int_t* idxp = mTT.mStripBegs[i];

      glArrayElement(*(idxp++));
      glArrayElement(*(idxp++));

      Int_t* tring_idxp = mTT.mStripTrings + (idxp - mTT.mStripEls);
      Int_t  n          = mTT.mStripLens[i] - 2;
      while (n-- > 0)
      {
	glNormal3fv(mTT.TriangleNormal(*tring_idxp));
	if (col_p) glColor4ubv(TriangleColor(*tring_idxp));
	++tring_idxp;
	glArrayElement(*(idxp++));
      }
      glEnd();
    }
  }
  else
  {
    glBegin(GL_TRIANGLES);
    Int_t*   T = mTT.Trings() + 3*t0;
    Float_t* N = mNArr + 3*t0;
    UChar_t* C = col_p ? mCArr + 4*t0 : 0;
    for (Int_t t=0; t<nt; ++t)
    {
      glNormal3fv(N); N += 3;
      if (col_p) { glColor4ubv(C);  C += 4; }
      glArrayElement(T[0]);
      glArrayElement(T[1]);
      glArrayElement(T[2]);
      T += 3;
    }
    glEnd();
  }
}

void TringTvor_GL_Rnr::render_mixed(Int_t t0, Int_t nt)
{
  const Bool_t norm_p = mNMode == TT::M_PerTriangle;
  const Bool_t col_p  = mCMode == TT::M_PerTriangle;
  const Bool_t tex_p  = mTMode == TT::M_PerTriangle;

  Float_t* na = norm_p ? mNArr + 3*t0 : 0;
  UChar_t* ca = col_p  ? mCArr + 4*t0 : 0;
  Float_t* ta = tex_p  ? mTArr + 6*t0 : 0;

  glBegin(GL_TRIANGLES);
  Int_t* T = mTT.Trings() + 3*t0;

  for (Int_t t = 0; t < nt; ++t)
  {
    if (norm_p) { glNormal3fv(na); na += 3; }
    if (col_p)  { glColor4ubv(ca); ca += 4; }
    if (tex_p)  
    {
      glTexCoord2fv(ta); ta += 2;
      glArrayElement(T[0]);
      glTexCoord2fv(ta); ta += 2;
      glArrayElement(T[1]);
      glTexCoord2fv(ta); ta += 2;
      glArrayElement(T[2]);
    }
    else
    {
      glArrayElement(T[0]);
      glArrayElement(T[1]);
      glArrayElement(T[2]);
    }
    T += 3;
  }
  glEnd();
}

void TringTvor_GL_Rnr::EndRender()
{
  glPopClientAttrib();
}

//==============================================================================
// Patches
//==============================================================================

bool TringTvor_GL_Rnr::faces_away(const TringTvor::Patch& p, const Double_t* e) const
{
  // Every normal of the cone points away from every point of the box's sphere.
  const Float_t *c = p.fCtrExt, *x = p.fCtrExt + 3, *a = p.fConeAxis;
  Double_t v[3] = { e[0] - e[3]*c[0], e[1] - e[3]*c[1], e[2] - e[3]*c[2] };
  Double_t vlen = std::sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
  Double_t r    = std::sqrt(x[0]*x[0] + x[1]*x[1] + x[2]*x[2]);
  return a[0]*v[0] + a[1]*v[1] + a[2]*v[2] < - p.fConeSin * vlen - e[3] * r * (1 + p.fConeSin);
}

void TringTvor_GL_Rnr::check_away(const TringTvor::Patch& p, const Double_t* e,
                                  const TString& what) const
{
  // Every triangle, by its winding, must face away from the eye.
  for (Int_t t = p.fFirstT; t < p.fFirstT + p.fNT; ++t)
  {
    const Int_t   *T  = &mTT.mTrings[3*t];
    const Float_t *v0 = &mTT.mVerts[3*T[0]], *v1 = &mTT.mVerts[3*T[1]], *v2 = &mTT.mVerts[3*T[2]];
    Double_t a[3] = { v1[0]-v0[0], v1[1]-v0[1], v1[2]-v0[2] };
    Double_t b[3] = { v2[0]-v0[0], v2[1]-v0[1], v2[2]-v0[2] };
    Double_t n[3] = { a[1]*b[2]-a[2]*b[1], a[2]*b[0]-a[0]*b[2], a[0]*b[1]-a[1]*b[0] };
    Double_t u[3] = { e[0]-e[3]*v0[0], e[1]-e[3]*v0[1], e[2]-e[3]*v0[2] };
    if (n[0]*u[0] + n[1]*u[1] + n[2]*u[2] > 0)
    {
      printf("TringTvor_GL_Rnr::check_away cone cull of %s not confirmed (triangle %d).\n",
             what.Data(), t);
      return;
    }
  }
}

void TringTvor_GL_Rnr::SelectVisiblePatches(RnrDriver* rd, std::vector<Int_t>& vis,
                                            const TString& what)
{
  const Int_t np = mTT.mPatches.size();
  vis.clear();
  if ( ! rd->CullingActive())
  {
    for (Int_t i = 0; i < np; ++i) vis.push_back(i);
    return;
  }

  GLint cull_mode, front_face;
  glGetIntegerv(GL_CULL_FACE_MODE, &cull_mode);
  glGetIntegerv(GL_FRONT_FACE,     &front_face);
  const bool cone = glIsEnabled(GL_CULL_FACE) && cull_mode != GL_FRONT && front_face == GL_CCW;
  Double_t eye[4];
  if (cone) rd->GetLocalEyePos(eye);

  Int_t n_culled = 0;
  for (Int_t i = 0; i < np; ++i)
  {
    const TringTvor::Patch &p = mTT.mPatches[i];
    if ( ! rd->IsVisible(p.fCtrExt))
    {
      if (rd->CheckMode()) rd->CheckCulled(GForm("%s patch %d", what.Data(), i), p.fCtrExt);
      ++n_culled;
    }
    else if (cone && p.fConeSin < 1 && faces_away(p, eye))
    {
      if (rd->CheckMode()) check_away(p, eye, GForm("%s patch %d", what.Data(), i));
      ++n_culled;
    }
    else
    {
      vis.push_back(i);
    }
  }
  rd->AddCulledParts(n_culled);
}

void TringTvor_GL_Rnr::RenderPatches(const std::vector<Int_t>& vis)
{
  // Adjacent patches are drawn as one run.
  const bool strips = bTriangleStrips;
  mCounts.clear();
  mOffsets.clear();
  Int_t t0 = 0, nt = 0, s0 = 0, ns = 0;
  for (size_t k = 0; k <= vis.size(); ++k)
  {
    const TringTvor::Patch *p = k < vis.size() ? &mTT.mPatches[vis[k]] : 0;
    if (p && nt > 0 && p->fFirstT == t0 + nt && ( ! strips || p->fFirstStrip == s0 + ns))
    {
      nt += p->fNT;
      ns += p->fNStrips;
      continue;
    }
    if (nt > 0)
    {
      if (mRenderMode == RM_Smooth && ! strips)
      {
        mCounts.push_back(3*nt);
        mOffsets.push_back(mTT.Trings() + 3*t0);
      }
      else if (mRenderMode == RM_Smooth) render_smooth(t0, nt, s0, ns);
      else if (mRenderMode == RM_Flat)   render_flat  (t0, nt, s0, ns);
      else                               render_mixed (t0, nt);
    }
    if (p) { t0 = p->fFirstT; nt = p->fNT; s0 = p->fFirstStrip; ns = p->fNStrips; }
  }
  if ( ! mCounts.empty())
  {
    glMultiDrawElements(GL_TRIANGLES, (const GLsizei*) mCounts.data(), GL_UNSIGNED_INT,
                        (const GLvoid**) mOffsets.data(), mCounts.size());
  }
}


//==============================================================================
// Static, all inclusive methods
//==============================================================================

void TringTvor_GL_Rnr::Render(TringTvor* ttvor)
{
  TringTvor_GL_Rnr rnr(ttvor);

  rnr.BeginSetup();
  rnr.SetupStandard();
  rnr.EndSetup();

  GL_Enum_Holder shade(GL_SHADE_MODEL, rnr.mRenderMode == RM_Flat ? GL_FLAT : GL_SMOOTH, glShadeModel);

  rnr.BeginRender();
  rnr.Render();
  rnr.EndRender();
}

void TringTvor_GL_Rnr::Render(TringTvor* ttvor, Bool_t smoothp, Bool_t colp, Bool_t texp)
{
  TringTvor_GL_Rnr rnr(ttvor);

  rnr.BeginSetup();
  if (smoothp)
    rnr.SetupStandardSmooth(colp, texp);
  else
    rnr.SetupStandardFlat(colp, texp);
  rnr.EndSetup();

  GL_Enum_Holder shade(GL_SHADE_MODEL, smoothp ? GL_SMOOTH : GL_FLAT, glShadeModel);

  rnr.BeginRender();
  rnr.Render();
  rnr.EndRender();
}

//------------------------------------------------------------------------------

void TringTvor_GL_Rnr::RenderCEBBox(const Float_t* x, Float_t f, Bool_t pushpop)
{
  // Render center-extents axis-aligned bounding-box.
  // f is additional scale of the extents.

  f *= 2;
  if (pushpop) glPushMatrix();
  glTranslatef(x[0]-x[3], x[1]-x[4], x[2]-x[5]);
  glScalef(f*x[3], f*x[4], f*x[5]);
  SphereTrings::UnitFrameBox();
  if (pushpop) glPopMatrix();
}
