// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef Gled_EyeInfo_H
#define Gled_EyeInfo_H

// Includes
#include <Glasses/ZMirEmittingEntity.h>
#include <Glasses/SaturnInfo.h>

#include <Eye/Ray.h>

class TSocket;

namespace gled {

class Saturn;
class Eye;
class EyeInfoVector;


class EyeInfo : public ZMirEmittingEntity
{
  MAC_RNR_FRIENDS(EyeInfo);
  friend class ZKing; friend class ZQueen; friend class ZSunQueen;
  friend class Saturn;
  friend class Eye;

public:
  typedef Eye*	(*EyeCreator_foo)(TSocket*, EyeInfo*, ZGlass*);

  enum PrivRayQN_e
  {
    PRQN_offset = RayNS::RQN_user_0,
    PRQN_text_message,
  };

private:
  TSocket            *hSocket;   //!
  Eye                *hEye;      //!
  EyeInfoVector      *hSelfEIV;  //!
  GMutex              hEyeMutex; //!

protected:
  ZLink<SaturnInfo>   mMaster; // X{gS} L{}

  void set_eye(Eye* eye);

  void emit_text_message_ray(const TString& s, InfoStream_e type);

public:
  EyeInfo(const Text_t* n="EyeInfo", const Text_t* t=0);
  virtual ~EyeInfo();

  virtual SaturnInfo* HostingSaturn() { return mMaster.get(); }

  // Virtuals exported from MEE
  virtual void Message(const TString& s);
  virtual void Warning(const TString& s);
  virtual void Error  (const TString& s);

#include "EyeInfo.h7"
  ClassDef(EyeInfo,1);
}; // endclass EyeInfo

} // endnamespace gled

#endif
