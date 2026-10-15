// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef GledCore_A_Rnr_H
#define GledCore_A_Rnr_H

#include <Eye/OptoStructs.h>

#include <vector>

namespace gled {

class ZGlass;
class RnrDriver;
class RnrScheme;
class RnrProgram;

class A_Rnr : public OptoStructs::A_View
{
  friend class RnrDriver;

public:
  struct NameStackEntry
  {
    A_Rnr *fRnr;
    void  *fUserData;
  };
  typedef NameStackEntry                 NSE_t;
  typedef std::list<NameStackEntry>           lNSE_t;
  typedef std::list<NameStackEntry>::iterator lNSE_i;
  typedef std::vector<NameStackEntry>         vNSE_t;

  struct Fl_Event
  {
    int     fEvent;
    int     fState;
    int     fKey;
    int     fButton;
    int     fButtons;
    int     fClicks;
    int     fIsClick;
    int     fX, fY;
    int     fDX, fDY;
    TString fText;

    bool    fIsKey;
    bool    fIsMouse;

    bool    fIsOverlay;

    lNSE_t  fNameStack;
    lNSE_i  fCurrentNSE;
    int     fNameStackSize;

    float   fZMin, fZMax;

    bool ns_empty() { return fNameStackSize > 0; }
    int  ns_size()  { return fNameStackSize;  }

    void dump();
  };

protected:
  RnrProgram* mProgram;
  bool        bOnePerRnrDriver;
  bool        bInRender;      // Between entry and exit of RnrDriver::Render.
  bool        bCycleReported;

public:
  void crs_links(RnrDriver* rd, RnrScheme* sch);
  void crs_self(RnrDriver* rd, RnrScheme* sch);
  void crs_elements(RnrDriver* rd, RnrScheme* sch);

public:
  A_Rnr() : OptoStructs::A_View(0), mProgram(0), bOnePerRnrDriver(false),
            bInRender(false), bCycleReported(false)
  {}
  virtual ~A_Rnr();

  virtual void AbsorbRay(Ray& ray);

  virtual void CreateRnrScheme(RnrDriver* rd, RnrScheme* sch);
  void         MarkProgramDirty();

  // Box as centre and half-extents, in the frame current at the lens's entry.
  virtual bool GetCullBox(RnrDriver* rd, Float_t* ctr_ext) { return false; }
  // True when a draw changes state for what is drawn after the lens.
  virtual bool HasOutsideEffects();

  virtual void PreDraw(RnrDriver* rd) = 0;
  virtual void Draw(RnrDriver* rd) = 0;
  virtual void PostDraw(RnrDriver* rd) = 0;

  virtual void CleanUp(RnrDriver* rd) {}

  virtual int  Handle(RnrDriver* rd, Fl_Event& ev) { return 0; }
  virtual void HandlePick(RnrDriver* rd, lNSE_t& ns, lNSE_i nsi) {}

  static const int sMaxRnrLevel;
};

/**************************************************************************/
// RnrOp, RnrScheme, RnrProgram
/**************************************************************************/

struct RnrOp
{
  enum Kind_e { K_PreDraw, K_Draw, K_PostDraw, K_Call };

  A_Rnr   *fRnr;
  UChar_t  fKind;
  Int_t    fPair;  // Index of the matching post or pre op, -1 if none.

  RnrOp(A_Rnr* r, UChar_t k) : fRnr(r), fKind(k), fPair(-1) {}

  bool operator==(const RnrOp& o) const { return fRnr == o.fRnr && fKind == o.fKind; }
};

typedef std::vector<RnrOp>  vRnrOp_t;

// Seven levels of ops, filled from RnrBits while a program is assembled, and
// the images other than the lens's own that the assembly read.
class RnrScheme
{
public:
  std::vector<vRnrOp_t>              fLevels;
  std::vector<OptoStructs::ZGlassImg*> fDeps;

  RnrScheme() : fLevels(A_Rnr::sMaxRnrLevel + 1) {}

  void Add(int level, A_Rnr* rnr, UChar_t kind)
  { if (level > 0) fLevels[level].push_back(RnrOp(rnr, kind)); }

  void AddDep(OptoStructs::ZGlassImg* img);

  // Concatenates the levels and pairs each pre op with the next post op of its renderer.
  void Flatten(vRnrOp_t& ops) const;
};

// Flat ops of one lens. A structural ray on the lens or on a dependency marks
// it dirty; it is reassembled before its next run.
class RnrProgram
{
  struct DepObserver : public OptoStructs::A_View
  {
    RnrProgram *fProg;

    DepObserver(OptoStructs::ZGlassImg* img, RnrProgram* p) :
      OptoStructs::A_View(img), fProg(p) {}
    virtual ~DepObserver();

    virtual void AbsorbRay(Ray& ray);
  };

  std::vector<DepObserver*> mDeps;

public:
  vRnrOp_t fOps;
  bool     bDirty;
  bool     bUnpaired;   // Some pre or post op has no partner.

  UInt_t   fCleanFrame; // Frame for which bClean was computed.
  bool     bClean;      // No op in the subtree has outside effects.

  RnrProgram() : bDirty(true), bUnpaired(false), fCleanFrame(0), bClean(false) {}
  ~RnrProgram();

  void Set(RnrDriver* rd, const RnrScheme& sch);
  bool Matches(const RnrScheme& sch) const;
};

/**************************************************************************/
// RnrMod & RnrModStore
/**************************************************************************/

struct RnrMod
{
  ZGlass      *fLens;
  A_Rnr       *fRnr;
  TimeStamp_t  fTringTS;

  RnrMod(ZGlass* l=0, A_Rnr* r=0, TimeStamp_t ts=0) :
    fLens(l), fRnr(r), fTringTS(ts) {}
};

struct RnrModStore
{
  RnrMod      *fRnrMod;
  TimeStamp_t  fTringTS;
  FID_t	       fFid;

  RnrModStore(FID_t f) : fRnrMod(0), fTringTS(0), fFid(f) {}

  ZGlass* lens() { return fRnrMod->fLens; }
  A_Rnr*  rnr()  { return fRnrMod->fRnr; }
};

template<class TL, class TR=A_Rnr>
struct RnrModStoreT : public RnrModStore
{
  RnrModStoreT()        : RnrModStore(TL::FID()) {}
  RnrModStoreT(FID_t f) : RnrModStore(f)         {}

  TL* lens() { return (TL*) fRnrMod->fLens; }
  TR* rnr()  { return (TR*) fRnrMod->fRnr; }
};

} // endnamespace gled

#endif
