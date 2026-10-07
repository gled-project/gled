// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef GledCore_Scene_GL_RNR_H
#define GledCore_Scene_GL_RNR_H

#include <Glasses/Scene.h>
#include <Rnr/GL/ZNode_GL_Rnr.h>

namespace gled {

class Scene_GL_Rnr : public ZNode_GL_Rnr {
private:
  void _init();

protected:
  Scene*	mScene;

public:
  Scene_GL_Rnr(Scene* idol) : ZNode_GL_Rnr(idol), mScene(idol) { _init(); }

  //virtual void PreDraw(RnrDriver* rd);
  //virtual void Draw(RnrDriver* rd);
  //virtual void PostDraw(RnrDriver* rd);

}; // endclass Scene_GL_Rnr

} // endnamespace gled

#endif
