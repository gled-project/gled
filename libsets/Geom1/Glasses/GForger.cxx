// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

//__________________________________________________________________________
// GForger
//
// Makes a random fractal terrain. Forge() puts the heights into the linked
// RectTerrain and, if an image is linked, a grayscale copy into the linked
// ZImage.
//
// The heights are fractional Brownian motion made by spectral synthesis. A
// Mesh x Mesh grid of frequencies gets Gaussian random amplitudes scaled by
// f^-(4 - Dimension), and an inverse FFT takes it back to a height field. The
// power spectrum of the heights then falls as f^-(8 - 2*Dimension). Dimension
// is the fractal dimension of the surface, from 2 (smooth) to 3 (rough). The
// height field is periodic, so copies of it tile without seams.
//
// The heights are scaled to [0, 1] and raised to Power. A Power above 1
// flattens the lowlands and sharpens the peaks.
//
// With Craters on, craters are added: CraterDensity craters per 1/100 of the
// map area, with radii from 0.4% to 10% of the map size (at least two cells),
// and the number of craters above radius r falling as r^-2. Each crater is a
// bowl with a raised rim. CraterHeight is the depth of the largest craters as
// a fraction of the height range; the depth of a crater is proportional to its
// radius. Craters wrap around the edges like the terrain.
//
// The result is scaled to [0, 1] again and multiplied by ZFactor in the
// terrain.
//
// Seed 0 takes a random seed. Forge() stores the seed it used in Seed, so
// forging again gives the same terrain. Mesh is rounded up to a power of two,
// and the value used is stored in Mesh.

#include "GForger.h"

#include <TMath.h>
#include <TRandom3.h>

#include <algorithm>
#include <cmath>
#include <complex>
#include <vector>

using namespace gled;

#include "GForger.c7"

namespace
{
  typedef std::complex<double> cplx_t;

  // In-place radix-2 FFT of n = 2^k values. sign = +1 gives the inverse
  // transform, without the 1/n normalisation.
  void fft_1d(cplx_t* a, int n, int sign)
  {
    for (int i = 1, j = 0; i < n; ++i)
    {
      int bit = n >> 1;
      for ( ; j & bit; bit >>= 1) j ^= bit;
      j ^= bit;
      if (i < j) std::swap(a[i], a[j]);
    }
    for (int len = 2; len <= n; len <<= 1)
    {
      const int    half = len / 2;
      const double ang  = sign * 2 * TMath::Pi() / len;
      for (int i = 0; i < n; i += len)
      {
        for (int k = 0; k < half; ++k)
        {
          const cplx_t w = std::polar(1.0, ang * k);
          const cplx_t u = a[i + k], v = a[i + k + half] * w;
          a[i + k]        = u + v;
          a[i + k + half] = u - v;
        }
      }
    }
  }

  // 2D FFT of an n x n grid stored row by row.
  void fft_2d(std::vector<cplx_t>& a, int n, int sign)
  {
    for (int y = 0; y < n; ++y)
      fft_1d(&a[y*n], n, sign);

    std::vector<cplx_t> col(n);
    for (int x = 0; x < n; ++x)
    {
      for (int y = 0; y < n; ++y) col[y] = a[y*n + x];
      fft_1d(&col[0], n, sign);
      for (int y = 0; y < n; ++y) a[y*n + x] = col[y];
    }
  }

  // Signed frequency of index i on an n-point grid.
  int freq(int i, int n) { return i <= n/2 ? i : i - n; }

  void normalize(std::vector<Float_t>& h)
  {
    auto mm = std::minmax_element(h.begin(), h.end());
    const Float_t min = *mm.first, delta = *mm.second - *mm.first;
    for (Float_t& z : h)
      z = delta > 0 ? (z - min) / delta : 0;
  }

  // Fractional Brownian motion on an n x n periodic grid.
  void spectral_synthesis(std::vector<Float_t>& h, int n, double dimension, TRandom& rnd)
  {
    // The amplitudes at (fx, fy) and (-fx, -fy) are complex conjugates, so the
    // inverse transform is real. The zero frequency, the mean height, stays 0.
    const double expo = -(4 - dimension) / 2;    // power of f^2
    std::vector<cplx_t> a(n*n);
    for (int y = 0; y < n; ++y)
    {
      const int fy = freq(y, n), cy = (n - y) % n;
      for (int x = 0; x < n; ++x)
      {
        const int fx = freq(x, n), cx = (n - x) % n;
        const int i  = y*n + x, c = cy*n + cx;
        const int f2 = fx*fx + fy*fy;
        if (c < i || f2 == 0)
          continue;
        const double amp = std::pow(double(f2), expo);
        if (c == i)
        {
          a[i] = amp * rnd.Gaus();
        }
        else
        {
          a[i] = amp * cplx_t(rnd.Gaus(), rnd.Gaus()) / std::sqrt(2.0);
          a[c] = std::conj(a[i]);
        }
      }
    }
    fft_2d(a, n, 1);

    h.resize(n*n);
    for (int i = 0; i < n*n; ++i)
      h[i] = a[i].real();
  }

  // Adds bowl-shaped craters with a rim to the heights h, n x n, periodic.
  void add_craters(std::vector<Float_t>& h, int n, double density, double max_depth,
                   TRandom& rnd)
  {
    // Radii are in units of the map size. The radius distribution is
    // dN/dr ~ r^-3 between r_min and r_max, sampled by inverting its integral.
    const double r_max = 0.1;
    const double r_min = std::min(std::max(0.004, 2.0 / n), r_max / 2);
    const double a = 1 / (r_min*r_min), b = 1 / (r_max*r_max);
    const int    count = TMath::Nint(100 * density);

    for (int k = 0; k < count; ++k)
    {
      const double r  = 1 / std::sqrt(a - rnd.Rndm() * (a - b));
      const double cx = rnd.Rndm() * n, cy = rnd.Rndm() * n;
      const double R  = r * n;
      const double depth = max_depth * r / r_max;
      const double rim   = 0.25 * depth;
      const int    ext   = (int) std::ceil(2 * R);
      const int    x0    = (int) std::floor(cx), y0 = (int) std::floor(cy);

      for (int dy = -ext; dy <= ext; ++dy)
      {
        const int y = ((y0 + dy) % n + n) % n;
        for (int dx = -ext; dx <= ext; ++dx)
        {
          const double d = std::hypot(x0 + dx - cx, y0 + dy - cy) / R;
          if (d >= 2)
            continue;
          const int    x = ((x0 + dx) % n + n) % n;
          const double t = (d - 1) / 0.35;
          h[y*n + x] += d < 1 ? depth * (d*d - 1) + rim : rim * std::exp(-t*t);
        }
      }
    }
  }
}

/**************************************************************************/

void GForger::_init()
{
  mZFactor = 1;

  mMesh = 256;
  mSeed = 0;

  mDimension = 2.15;
  mPower     = 1.2;

  bCraters       = false;
  mCraterDensity = 1;
  mCraterHeight  = 0.5;
}

/**************************************************************************/

void GForger::Forge()
{
  static const Exc_t _eh("GForger::Forge ");

  // Forge() runs in a detached thread, without the lock. Take a copy of the
  // parameters.
  Int_t        n, seed;
  Float_t      dimension, power, zfac, cr_density, cr_height;
  Bool_t       craters;
  ZImage      *image;
  RectTerrain *terrain;
  {
    GLensReadHolder _rlck(this);
    n          = mMesh;
    seed       = mSeed;
    dimension  = mDimension;
    power      = mPower;
    zfac       = mZFactor;
    craters    = bCraters;
    cr_density = mCraterDensity;
    cr_height  = mCraterHeight;
    image      = mImage.get();
    terrain    = mTerrain.get();
  }

  if (image == 0 && terrain == 0)
    throw _eh + "neither Image nor Terrain is set.";

  Int_t mesh = 4;
  while (mesh < n) mesh <<= 1;
  n = mesh;

  if (seed == 0)
  {
    TRandom3 seeder(0);
    seed = 1 + (Int_t) seeder.Integer(2147483646);
  }

  TRandom3 rnd(seed);
  std::vector<Float_t> h;
  spectral_synthesis(h, n, dimension, rnd);
  normalize(h);
  if (power != 1)
  {
    for (Float_t& z : h)
      z = std::pow(z, power);
  }
  if (craters)
  {
    add_craters(h, n, cr_density, cr_height, rnd);
    normalize(h);
  }

  {
    GLensWriteHolder _wlck(this);
    SetMesh(n);
    SetSeed(seed);
  }

  if (terrain != 0)
  {
    GLensWriteHolder _wlck(terrain);
    terrain->SetFromArray(n, n, &h[0], zfac);
  }

  if (image != 0)
  {
    GLensWriteHolder _wlck(image);
    image->SetupAsCanvas(n, n, 1, false);
    ZImage::sILMutex.Lock();
    image->bind();
    for (Int_t y = 0; y < n; ++y)
      for (Int_t x = 0; x < n; ++x)
        image->set_byte(x, y, (UChar_t) TMath::Nint(255 * h[y*n + x]));
    image->unbind();
    ZImage::sILMutex.Unlock();
    image->StampReqTring(FID());
  }
}
