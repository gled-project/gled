// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef GledCore_Gled_H
#define GledCore_Gled_H

// Includes
#include <Gled/GledTypes.h>
#include <Gled/GMutex.h>

class TApplication;
class TRint;

class Fl_SWM_Manager;
class Fl_Window;
class Fl_Tile;
class Fl_Browser;

namespace gled {

class Saturn;
class SaturnInfo;
class EyeInfo;
class ShellInfo;
class Forest;
class Mountain;
class Eye;
class ZMIR;

class GCondition;
class GThread;
class GKeyRSA;


class Gled
{
protected:
  void next_arg_or_die(lStr_t& args, lStr_i& i, bool allow_single_minus=false);

  TString       mCmdName;       // X{GS}
  lStr_t        mArgs;

  SaturnInfo*	mSaturnInfo;	// X{g}
  Saturn*     mSaturn;	// X{g}
  Bool_t      bIsSun;		// X{G}
  GKeyRSA*    mSunKey;	// X{g}

  Bool_t	bQuit;		// X{G}
  Bool_t	bHasPrompt;	// X{G}
  Bool_t	bShowSplash;	// X{G}
  Bool_t	bPreExec;       // X{G}
  TString	mPreExecString; // X{G}
  Bool_t	bAutoSpawn;	// X{G}
  Bool_t        bEarlySrvSock;  // X{G}
  Bool_t	bAllowMoons;	// X{G}
  Bool_t        bIsDaemon;      // X{G}
  TString       mRootWebDisplay;// X{G}
  FILE         *mPidFile;

  Bool_t        bSetupFinished;
  std::list<ZMIR*>   mAfterSetupMirs;

  Bool_t	bRootAppRunning;// X{G}
  TApplication *mRootApp;       // X{g}

  GMutex	mLoggingMutex;
  TString	mLogFileName;
  FILE*		mLogFile;
  TString	mOutFileName;
  TString	mErrFileName;

  TString       mIncDir;        // X{G}
  TString       mLibDir;        // X{G}
  TString       mDataDir;       // X{G}
  TString       mHomeDir;       // X{G}

  TString	mAuthDir;	 // X{GS}
  TString	mDefEyeIdentity; // X{GS}

  TString	mRenderers;      // X{GS}

  GCondition*	mExitCondVar;
  GThread*      mRootAppThread;  // X{g}
  GThread*      mExitThread;

  static Int_t  sExitStatus;

public:
  Gled();

  virtual void AddArgument(const char* arg);
  virtual void ReadArguments(int argc, char **argv);

  virtual void ParseArguments(Bool_t allow_daemon);

  virtual void Initialize();
  virtual void InitLogging();
  virtual void InitGledCore();

  virtual void ProcessCmdLineMacros();

  virtual void StopLogging();

  virtual ~Gled();

  void SetExitCondVar(GCondition* cond) { mExitCondVar = cond; }

  void PreExec();

  void SpawnSunOrSaturn();
  void SpawnSun();
  void SpawnSaturn();

  void        AfterSetup(ZMIR* mir);
  void        ShootAfterSetupMirs();

  void        CheckAuthDir();
  const char* PubKeyFile(TString& id);
  const char* PrivKeyFile(TString& id);
  const char* GetPubKeyFile(TString& id, Bool_t use_exc=true);
  const char* GetPrivKeyFile(TString& id, Bool_t use_exc=true);

  Bool_t IsIdentityInGroup(const char* id, const char* group);

  void WaitUntilQueensLoaded();
  void AllowMoonConnections();

  virtual void  Run();
          void  Exit(Int_t status=0);
  static  void* Exit_tl(void*);
  virtual void  ExitVirtual();

  virtual Int_t LoadLibSet(const Text_t* lib_set);
  virtual Int_t LoadLibSet(LID_t lid);

  virtual void SetDebugLevel(Int_t d=0);

  virtual void info(const char* s);
  virtual void message(const char* s);
  virtual void warning(const char* s);
  virtual void error(const char* s);

  virtual Bool_t HasGUILibs() const { return false; }

  virtual EyeInfo* SpawnEye(const TString& libset, const TString& eyector);
  virtual EyeInfo* SpawnEye(EyeInfo* ei, ZGlass* ud,
			    const TString& libset, const TString& eyector)
  { return 0; }

  static void AssertLibSet(const Text_t* lib_set);

  static void AssertMacro(const Text_t* mac);
  static void Macro(const Text_t* mac);
  static void LoadMacro(const Text_t* mac);

  GThread*     SpawnRootAppThread(const TString& name_prefix);
  static void* RootApp_runner_tl(void*);
  static void  RootApp_cleanup_tl(void*);

  void         SpawnTimeBeatThread();
  static void* TimeBeat_tl(void*);

  virtual void LockFltk()   {}
  virtual void UnlockFltk() {}
  virtual void SetInputMethod(Bool_t on) {} // X input method, eg ibus, on or off.

  static Gled* theOne;
  static Int_t GetExitStatus();

#include "Gled.h7"
  ClassDef(Gled, 0);
}; // endclass Gled

} // endnamespace gled

#endif
