// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef GledCore_Saturn_H
#define GledCore_Saturn_H

#include <Gled/GledTypes.h>
#include <Glasses/ZGlass.h>
#include <Stones/ZMIR.h>

#include <Gled/GMutex.h>
#include <Gled/GSelector.h>
#include <Gled/GCondition.h>
#include <Gled/GTime.h>

class TServerSocket; class TSocket;
class TMessage;

namespace gled {

class ZGod;
class ZKing; class ZFireKing;
class ZSunQueen; class ZQueen; class ZFireQueen;
class SaturnInfo; class EyeInfo; class Ray; class EyeInfoVector;
class TextMessage;

class Mountain;
class ZHistoManager;
class GThread;

class Saturn : public An_ID_Demangler
{
  friend class Gled;
  friend class ZKing;
  friend class ZQueen; friend class ZSunQueen;
  friend class SaturnInfo;

  typedef std::list<SaturnInfo*>           lpSaturnInfo_t;
  typedef std::list<SaturnInfo*>::iterator lpSaturnInfo_i;
  typedef std::list<EyeInfo*>              lpEyeInfo_t;
  typedef std::list<EyeInfo*>::iterator    lpEyeInfo_i;

  typedef std::list<TSocket*>              lpSocket_t;
  typedef std::list<TSocket*>::iterator    lpSocket_i;

public:
  struct SocketInfo
  {
    enum OtherSide_e { OS_Moon, OS_Eye };  // What on other side ...
    OtherSide_e	fWhat;
    ZGlass*	fLensRep;

    SaturnInfo* get_moon() { return (SaturnInfo*)fLensRep; }
    EyeInfo*	get_eye()  { return (EyeInfo*)fLensRep; }

    SocketInfo(OtherSide_e w, ZGlass* g) : fWhat(w), fLensRep(g) {}
  };

  typedef std::unordered_map<TSocket*, SocketInfo>	   hSock2SocketInfo_t;
  typedef std::unordered_map<TSocket*, SocketInfo>::iterator hSock2SocketInfo_i;

  typedef std::multimap<GTime, ZMIR*>           mTime2MIR_t;
  typedef std::multimap<GTime, ZMIR*>::iterator mTime2MIR_i;

protected:
  GMutex		mIDLock;	// X{r} ... must allow locking to eyez
  GMutex		mEyeLock;	// sending to eyes
  GMutex		mMoonLock;	// sending to moons
  GMutex		mMasterLock;	// sending to master
  GMutex		mRulingLock;	// Exec in kings & queens

  GSelector		mSelector;	// fd select wrapper for sockets

  ZGod*			mGod;		// X{g}
  ZKing*		mSunKing;	// X{g}
  ZSunQueen*		mSunQueen;	// X{g}
  ZKing*		mKing;		// X{g}
  ZFireKing*		mFireKing;	// X{g}
  ZFireQueen*		mFireQueen;	// X{g}
  SaturnInfo*		mSunInfo;	// X{g}
  SaturnInfo*		mSaturnInfo;	// X{g}
  bool			bSunAbsolute;	// X{g}

  Int_t			mQueenLoadNum;	// X{gS}
  GCondition		mQueenLoadCnd;	// X{r}

  Mountain*		mChaItOss;	// X{g}
  hID2pZGlass_t		mIDHash;
  hSock2SocketInfo_t	mSock2InfoHash;
  lpSaturnInfo_t	mMoons;
  lpEyeInfo_t		mEyes;
  TServerSocket*	mServerSocket;

  // Server Thread
  GThread*		mServerThread;
  GThread*              mShutdownThread;

  Bool_t		bAllowMoons;	// X{g}


  // Detached lens threads


  struct DetachedThreadInfo
  {
    ZGlass  *fLens;
    GThread *fThread;

    DetachedThreadInfo() : fLens(0), fThread(0) {}
    DetachedThreadInfo(ZGlass *l, GThread *t) : fLens(l), fThread(t) {}
  };

  typedef std::list<DetachedThreadInfo>           lDetachedThreadInfo_t;
  typedef std::list<DetachedThreadInfo>::iterator lDetachedThreadInfo_i;

  class DetachedThreadsPerLens
  {
    struct Thread
    {
      GThread               *fThread;
      lDetachedThreadInfo_i  fMainIter;

      Thread(GThread* t, lDetachedThreadInfo_i i) : fThread(t), fMainIter(i) {}
    };
    std::list<Thread> fThreads;
  public:
    DetachedThreadsPerLens() {}

    bool                  IsEmpty() const;
    GThread*              GetLastThread() const;
    void                  PushBack(GThread* t, lDetachedThreadInfo_i i);
    lDetachedThreadInfo_i PopBack();
    lDetachedThreadInfo_i FindAndRemove(GThread* t);
  };

  typedef std::unordered_map<ZGlass*, DetachedThreadsPerLens>           hpZGlass2DetachedThreadPerLens_t;
  typedef std::unordered_map<ZGlass*, DetachedThreadsPerLens>::iterator hpZGlass2DetachedThreadPerLens_i;

  lDetachedThreadInfo_t             mDetachedThreadsList;
  hpZGlass2DetachedThreadPerLens_t  mDetachedThreadsHash;
  GMutex                            mDetachedMirMutex;

public:
  void register_detached_thread  (ZGlass *lens, GThread *thread);
  void unregister_detached_thread(ZGlass *lens, GThread *thread);
protected:
  bool cancel_and_join_thread    (ZGlass* lens, GThread* thread);


  // Saturn services ... preliminary
  ZHistoManager*	pZHistoManager;

  static int start_threads(Saturn *saturn);
  int  create_server_socket();
  int  start_server();
  int  stop_server();
  int  start_shooters();
  int  stop_shooters();

  int  stop_detached_threads();

  void socket_closed(TSocket* sock);
  void wipe_moon(SaturnInfo* moon, bool notify_sunqueen_p);
  void wipe_eye(EyeInfo* eye, bool notify_sunqueen_p);

  void fix_fire_king_id(SaturnInfo* si);
  void create_kings(const char* king, const char* whore_king);
  void arrival_of_kings(TMessage* m);

  void Enlight(ZGlass* glass, ID_t);
  void Reflect(ZGlass* glass);
  void Freeze(ZGlass* glass);
  void Endark(ZGlass* glass);

  Bool_t IsMoon(SaturnInfo* si);
  void   CopyMoons(lpSaturnInfo_t& list);

  Int_t	SockSuck();	// Called constantly from ServerThread
  SaturnInfo* FindRouteTo(SaturnInfo* target);

  void AcceptWrapper(TSocket* newsocket);
  void Accept(TSocket* newsocket);
  void finalize_moon_connection(SaturnInfo* si);
  void finalize_eye_connection(EyeInfo* ei);

  Int_t	Manage(TSocket* sock);

public:
  Saturn();
  virtual ~Saturn();

  static TString   HandleClientSideSaturnHandshake(TSocket*& socket);
  static TMessage* HandleClientSideMeeConnection(TSocket* socket, ZMirEmittingEntity* mee);

  void	      Create(SaturnInfo* si);
  SaturnInfo* Connect(SaturnInfo* si);
  TSocket*    MakeSocketPairAndAccept(const TString& name);
  void        OpenServerSocket();
  void	      AllowMoons();
  void	      Shutdown();

  void        LockMIRShooters(bool wait_until_queue_empty=false);
  void        UnlockMIRShooters();

  virtual ZGlass* DemangleID(ID_t id);

  Int_t Freeze();
  Int_t UnFreeze();

  // Saturn services
  ZHistoManager* GetZHistoManager();

  static const Int_t s_Gled_Protocol_Version;

  /**************************************************************************/
  // MIR and MIR Result Request handling
  /**************************************************************************/

protected:

  // MIR Result Request registration and storage
  struct mir_rr_info
  {
    GCondition*  cond;
    ZMIR_RR*     mir_rr;
    mir_rr_info(GCondition* c) : cond(c), mir_rr(0) {}
  };

  typedef std::unordered_map<UInt_t, mir_rr_info>			hReqHandle2MirRRInfo_t;
  typedef std::unordered_map<UInt_t, mir_rr_info>::iterator	hReqHandle2MirRRInfo_i;

  hReqHandle2MirRRInfo_t 	mBeamReqHandles;

  GMutex		mBeamReqHandleMutex;
  UInt_t		mLastBeamReqHandle;

  UInt_t    register_mir_result_request(GCondition* cond);
  ZMIR_RR*  query_mir_result(UInt_t req_handle);
  void	    handle_mir_result(UInt_t req_handle, ZMIR* mirp);

  GThread*		mMIRShootingThread;
  GCondition		mMIRShootingCnd;
  GMutex		mMIRShooterRoutingLock;
  std::list<ZMIR*>		mMIRShootingQueue;

  GThread*		mDelayedMIRShootingThread;
  GCondition		mDelayedMIRShootingCnd;
  mTime2MIR_t		mDelayedMIRShootingQueue;

  void     markup_posted_mir(ZMIR& mir, ZMirEmittingEntity* caller=0);
  void     post_mir(std::unique_ptr<ZMIR>& mir, ZMirEmittingEntity* caller=0);
  void     shoot_mir(std::unique_ptr<ZMIR>& mir, ZMirEmittingEntity* caller,
		     bool use_own_thread=false);
  void     delayed_shoot_mir(std::unique_ptr<ZMIR>& mir, ZMirEmittingEntity* caller,
			     const GTime& at_time);

  void     mir_shooter();
  void     delayed_mir_shooter();

  void     generick_shoot_mir_result(ZMIR& mir, const Text_t* exc, TBuffer* buf);

public:

  void     PostMIR(std::unique_ptr<ZMIR>& mir);
  void     PostMIR(ZMIR* mir);

  void     ShootMIR(std::unique_ptr<ZMIR>& mir, bool use_own_thread=false);
  void     ShootMIR(ZMIR* mir, bool use_own_thread=false);
  void     DelayedShootMIR(std::unique_ptr<ZMIR>& mir, const GTime& at_time);
  void     DelayedShootMIR(ZMIR* mir, const GTime& at_time);
  ZMIR_RR* ShootMIRWaitResult(std::unique_ptr<ZMIR>& mir, bool use_own_thread=false);
  ZMIR_RR* ShootMIRWaitResult(ZMIR* mir, bool use_own_thread=false);

  void     ShootMIRResult(TBuffer& buf);

  // Internal MIR handling

protected:
  void report_mir_pre_demangling_error(ZMIR& mir, TString error);
  void report_mir_post_demangling_error(ZMIR& mir, TString error);

  void RouteMIR(std::unique_ptr<ZMIR>& mir);
  void UnfoldMIR(std::unique_ptr<ZMIR>& mir);
  void ExecMIR(ZMIR* mir, bool lockp=true);
  void ExecMIR(std::unique_ptr<ZMIR>& mir, bool lockp=true);
  void ExecDetachedMIR(std::unique_ptr<ZMIR>& mir);

  void ForwardMIR(ZMIR& mir, SaturnInfo* route);
  void BroadcastMIR(ZMIR& mir, lpSaturnInfo_t& moons);
  void BroadcastBeamMIR(ZMIR& mir, lpSaturnInfo_t& moons);


  /**************************************************************************/
  // Ray handling ... viewer notifications.
  /**************************************************************************/

 protected:
  typedef std::pair<Ray*, EyeInfoVector*> RayQueueEntry_t;
  typedef std::list<RayQueueEntry_t>      RayQueue_t;

  Bool_t                bAcceptsRays;

  GThread*              mRayEmittingThread;
  GCondition            mRayEmittingCnd;
  RayQueue_t            mRayEmittingQueue;

  void ray_emitter();

 public:

  Bool_t AcceptsRays() const { return bAcceptsRays; }

  void   Shine(std::unique_ptr<Ray>& ray, EyeInfoVector* eiv);

  /**************************************************************************/
  // Internal thread structures and functions
  /**************************************************************************/

private:

  // ThreadInfo structures (passed via void* to threads)

  struct new_connection_ti
  {
    Saturn*		sat;
    TSocket*		sock;
    new_connection_ti(Saturn* s, TSocket* so) : sat(s), sock(so) {}
  };

  // Thread functions

  static void* tl_SaturnFdSucker(Saturn *s);
  static void* tl_SaturnAcceptor(new_connection_ti *ss);
  static void* tl_MIR_Router(Saturn* sat);
  static void* tl_MIR_DetachedExecutor(Saturn* sat);
  static void  tl_MIR_DetachedCleanUp(Saturn* sat);

  static void* tl_MIR_Shooter(Saturn* s);
  static void* tl_Delayed_MIR_Shooter(Saturn* s);
  static void* tl_Ray_Emitter(Saturn* s);

public:
#include "Saturn.h7"
  ClassDef(Saturn, 0);
}; // endclass Saturn

/**************************************************************************/


inline ZGlass* Saturn::DemangleID(ID_t id)
{
  mIDLock.Lock();
  hID2pZGlass_i i = mIDHash.find(id);
  ZGlass *ret = (i != mIDHash.end()) ? i->second : 0;
  mIDLock.Unlock();
  return ret;
}


/**************************************************************************/

} // endnamespace gled

#endif
