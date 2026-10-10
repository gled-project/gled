// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef GTS_GTS_H
#define GTS_GTS_H

// gts-cxx needs C++23; only GledGTS sources include this header.
#include <gts/all.h>

#include <memory>

class TString;

namespace gled
{
  class ZTrans;

  namespace GTS
  {
    // A surface and the Geometrium that holds it, owned together.
    class Mesh
    {
    public:
      Mesh(); // with an empty surface
      Mesh(std::unique_ptr<gts::Geometrium> g, gts::Surface& s) : m_geo(std::move(g)), m_surf(&s) {}

      gts::Geometrium&       geo()        { return *m_geo; }
      const gts::Geometrium& geo()  const { return *m_geo; }
      gts::Surface&          surf()       { return *m_surf; }
      const gts::Surface&    surf() const { return *m_surf; }

      // A copy of the surface in a new Geometrium.
      Mesh* Copy() const;

    private:
      std::unique_ptr<gts::Geometrium> m_geo;
      gts::Surface*                    m_surf;
    };
  }

  void InvertSurface(gts::Surface& s);

  void TransformSurfaceVertices(gts::Surface& s, const ZTrans& t);
  void RotateSurfaceVertices(gts::Surface& s, const ZTrans& t);

  void WriteSurfaceToFile(const gts::Surface& s, const TString& file);
} // endnamespace gled


#endif
