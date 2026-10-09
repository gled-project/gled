// One Saturn of run_cluster.py: role and run directory from CLUSTER_ROLE and
// CLUSTER_RUNDIR; phases go through go.<phase> and done/fail.<role>.<phase> files.

#include <Gled/GledNS.h>
#include "gled_globals.C"

#include <TSystem.h>
#include <TBufferFile.h>

#include <deque>
#include <fstream>
#include <map>
#include <memory>

using namespace gled;

TString cn_rundir, cn_role;

void cn_log(const TString& s)
{
  printf("CLUSTER[%s] %s\n", cn_role.Data(), s.Data());
  fflush(stdout);
}

TString cn_file(const TString& name) { return cn_rundir + "/" + name; }

void cn_touch(const TString& name, const TString& text = "ok")
{
  std::ofstream f(cn_file(name).Data());
  f << text << "\n";
}

bool cn_exists(const TString& name)
{
  // AccessPathName() returns false when the path exists.
  return ! gSystem->AccessPathName(cn_file(name));
}

void cn_wait_for(const TString& name, int timeout_s = 300)
{
  for (int i = 0; i < timeout_s * 10; ++i)
  {
    if (cn_exists(name)) return;
    if (cn_exists("go.quit")) throw Exc_t("quit requested while waiting for " + name);
    gSystem->Sleep(100);
  }
  throw Exc_t("timeout waiting for " + name);
}

void cn_post(ZMIR* mir)
{
  g_saturn->PostMIR(mir);
  delete mir;
}

//==============================================================================
// Lookup helpers
//==============================================================================

TString cn_qkey(ZQueen* q)
{
  if (q == 0) return "none";
  ZKing      *k  = q->GetKing();
  SaturnInfo *si = k ? k->GetSaturnInfo() : 0;
  return TString::Format("%s/%s", si ? si->GetName() : "?", q->GetName());
}

ZQueen* cn_find_queen(const TString& name)
{
  lpZGlass_t kings; g_saturn->GetGod()->CopyList(kings);
  for (auto ki : kings)
  {
    ZKing* k = dynamic_cast<ZKing*>(ki);
    if (k == 0) continue;
    ZQueen* q = dynamic_cast<ZQueen*>(k->GetElementByName(name));
    if (q) return q;
  }
  throw Exc_t("queen not found: " + name);
}

template <class GLASS>
GLASS* cn_child(AList* l, const TString& name)
{
  GLASS* g = dynamic_cast<GLASS*>(l->GetElementByName(name));
  if (g == 0) throw Exc_t("no element '" + name + "' in " + l->GetName());
  return g;
}

bool cn_wait_ruling(ZQueen* q, int timeout_s = 60)
{
  for (int i = 0; i < timeout_s * 10; ++i)
  {
    if (q->GetRuling()) return true;
    gSystem->Sleep(100);
  }
  return false;
}

//==============================================================================
// Dump
//==============================================================================

const char* cn_light(ZKing* k)
{
  switch (k->GetLightType())
  {
    case ZKing::LT_Moon: return "moon";
    case ZKing::LT_Sun:  return "sun";
    case ZKing::LT_Fire: return "fire";
    default:             return "undef";
  }
}

TString cn_hash(ZGlass* g)
{
  TBufferFile b(TBuffer::kWrite);
  g->Streamer(b);
  // FNV-1a over the streamed bytes.
  ULong64_t h = 1469598103934665603ull;
  const unsigned char* p = (const unsigned char*) b.Buffer();
  for (Int_t i = 0; i < b.Length(); ++i) { h ^= p[i]; h *= 1099511628211ull; }
  return TString::Format("%016llx", h);
}

struct CnWalk
{
  ZQueen                     *fQueen;
  std::map<ZGlass*, TString>  fPath;
  std::deque<ZGlass*>         fTodo;

  TString ref(ZGlass* t, const TString& new_path)
  {
    if (t == 0) return "null";
    if (t->GetQueen() != fQueen)
      return TString::Format("ext:%s:%s:%s", cn_qkey(t->GetQueen()).Data(),
                             t->IsA()->GetName(), t->GetName());
    auto i = fPath.find(t);
    if (i != fPath.end()) return i->second;
    fPath[t] = new_path;
    fTodo.push_back(t);
    return new_path;
  }
};

void cn_dump_queen(std::ostream& os, ZQueen* q)
{
  CnWalk w;
  w.fQueen = q;
  w.fPath[q] = "Q";
  w.fTodo.push_back(q);

  os << "queen " << cn_qkey(q) << " ids_used=" << q->GetIDsUsed()
     << " mandatory=" << (q->GetMandatory() ? 1 : 0) << "\n";

  while ( ! w.fTodo.empty())
  {
    ZGlass* g = w.fTodo.front(); w.fTodo.pop_front();
    TString p = w.fPath[g];

    TString line = TString::Format("lens %s | %s | %s | %s", p.Data(),
                                   g->IsA()->GetName(), g->GetName(), g->GetTitle());

    ZGlass::lLinkRep_t lreps;
    g->CopyLinkReps(lreps);
    for (auto& lr : lreps)
    {
      TString ln = lr.fLinkInfo->fName;
      line += " | @" + ln + "=" + w.ref(lr.fLinkRef, p + "/@" + ln);
    }

    AList* l = g->AsAList();
    if (l)
    {
      lpZGlass_t els; l->CopyList(els);
      line += " | [";
      int idx = 0;
      for (auto e : els)
      {
        TString ep = e ? TString::Format("%s/%d:%s", p.Data(), idx, e->GetName()) : TString("");
        line += (idx ? "," : "") + w.ref(e, ep);
        ++idx;
      }
      line += "]";
    }

    os << line << " | hash=" << cn_hash(g) << " | id=" << g->GetSaturnID() << "\n";
  }
}

void cn_dump(const TString& fname)
{
  std::ofstream os(cn_file(fname).Data());

  os << "saturn " << g_saturn->GetSaturnInfo()->GetName()
     << " sun_absolute=" << (g_saturn->GetSunAbsolute() ? 1 : 0) << "\n";

  lpZGlass_t kings; g_saturn->GetGod()->CopyList(kings);
  for (auto ki : kings)
  {
    ZKing* k = dynamic_cast<ZKing*>(ki);
    if (k == 0) continue;
    SaturnInfo* si = k->GetSaturnInfo();
    os << "king " << (si ? si->GetName() : "?") << " light=" << cn_light(k)
       << " | id=" << k->GetSaturnID() << "\n";

    lpZGlass_t queens; k->CopyList(queens);
    for (auto qi : queens)
    {
      ZQueen* q = dynamic_cast<ZQueen*>(qi);
      if (q == 0) continue;
      if (q->GetRuling())
        cn_dump_queen(os, q);
      else
        os << "shell " << cn_qkey(q) << "\n";
    }
  }
}

//==============================================================================
// Phase actions
//==============================================================================

void cn_build()
{
  if (cn_role == "sun")
  {
    ZQueen* q = new ZQueen(1024, "Shared", "harness: mandatory queen of the sun");
    g_sun_king->Enthrone(q);
    q->SetMandatory(true);

    ZList* top = new ZList("top");         q->CheckIn(top);    q->Add(top);
    ZNode* n1  = new ZNode("n1");          q->CheckIn(n1);     top->Add(n1);
    ZNode* n2  = new ZNode("n2");          q->CheckIn(n2);     n1->Add(n2);
    ZNodeLink* nl = new ZNodeLink("nl");   q->CheckIn(nl);     top->Add(nl);
    nl->SetLens(n2);
    ZNode* doomed = new ZNode("doomed");   q->CheckIn(doomed); top->Add(doomed);

    ZQueen* od = new ZQueen(1024, "OnDemand", "harness: non-mandatory queen of the sun");
    g_sun_king->Enthrone(od);
    ZList* otop = new ZList("od-top");     od->CheckIn(otop);  od->Add(otop);
    ZNode* odn  = new ZNode("odn");        od->CheckIn(odn);   otop->Add(odn);
  }
  else if (cn_role == "m1")
  {
    ZQueen* q = new ZQueen(1024, "M1Shared", "harness: mandatory queen of m1");
    g_king->Enthrone(q);
    q->SetMandatory(true);
    ZList* top = new ZList("m1-top");      q->CheckIn(top);    q->Add(top);
    ZNode* a   = new ZNode("m1-a");        q->CheckIn(a);      top->Add(a);

    ZNode* f = new ZNode("fire-local");
    g_fire_queen->CheckIn(f); g_fire_queen->Add(f);
  }
}

void cn_mirror()
{
  if (cn_role == "m1b")
  {
    ZQueen* od = cn_find_queen("OnDemand");
    cn_post(g_fire_king->S_RequestQueenMirroring(od));
    if ( ! cn_wait_ruling(od))
      throw Exc_t("OnDemand did not become ruling on m1b");
  }
}

void cn_modify()
{
  if (cn_role == "sun")
  {
    ZQueen* sh  = cn_find_queen("Shared");
    ZList*  top = cn_child<ZList>(sh, "top");
    cn_post(sh->S_RemoveLens(cn_child<ZNode>(top, "doomed")));
  }
  else if (cn_role == "m1")
  {
    ZQueen* q = cn_find_queen("M1Shared");
    ZNode*  a = cn_child<ZNode>(cn_child<ZList>(q, "m1-top"), "m1-a");
    cn_post(a->S_Move3LF(1, 2, 3));
  }
  else if (cn_role == "m1b")
  {
    ZQueen* sh  = cn_find_queen("Shared");
    ZList*  top = cn_child<ZList>(sh, "top");

    std::unique_ptr<ZMIR> att(top->S_Add(0));
    std::unique_ptr<ZMIR> inst(sh->S_InstantiateWAttach(ZNode::FID().fLid, ZNode::FID().fCid,
                                                        "born-on-m1b"));
    inst->ChainMIR(att.get());
    std::unique_ptr<ZMIR_RR> res(g_saturn->ShootMIRWaitResult(inst));
    if (res->HasException())
      throw Exc_t("InstantiateWAttach failed: " + TString(res->fException.Data()));

    ZQueen* od  = cn_find_queen("OnDemand");
    ZNode*  odn = cn_child<ZNode>(cn_child<ZList>(od, "od-top"), "odn");
    cn_post(odn->S_SetName("odn-renamed-by-m1b"));
  }
  else if (cn_role == "m2")
  {
    ZQueen*    sh  = cn_find_queen("Shared");
    ZList*     top = cn_child<ZList>(sh, "top");
    ZNode*     n1  = cn_child<ZNode>(top, "n1");
    ZNodeLink* nl  = cn_child<ZNodeLink>(top, "nl");
    cn_post(nl->S_SetLens(n1));
    cn_post(n1->S_Move3LF(4, 5, 6));
    cn_post(n1->S_SetName("n1-renamed-by-m2"));

    ZQueen* q = cn_find_queen("M1Shared");
    cn_post(cn_child<ZList>(q, "m1-top")->S_SetTitle("touched-by-m2"));
  }
}

//==============================================================================
// Scenario "scale": a sun, fan-out moons and a chain; see run_cluster.py.
//==============================================================================

TString cn_env(const char* name)
{
  const char* v = gSystem->Getenv(name);
  return v ? v : "";
}

void cn_instantiate_into(ZQueen* q, ZList* list, const TString& name)
{
  std::unique_ptr<ZMIR> att(list->S_Add(0));
  std::unique_ptr<ZMIR> inst(q->S_InstantiateWAttach(ZNode::FID().fLid, ZNode::FID().fCid, name));
  inst->ChainMIR(att.get());
  std::unique_ptr<ZMIR_RR> res(g_saturn->ShootMIRWaitResult(inst));
  if (res->HasException())
    throw Exc_t("InstantiateWAttach into " + TString(q->GetName()) + " failed: " +
                TString(res->fException.Data()));
}

void sc_build()
{
  if (cn_role == "sun")
  {
    ZQueen* q = new ZQueen(4096, "Shared", "harness: mandatory queen of the sun");
    g_sun_king->Enthrone(q);
    q->SetMandatory(true);
    ZList* top = new ZList("top");  q->CheckIn(top);  q->Add(top);
    ZNode* hot = new ZNode("hot");  q->CheckIn(hot);  top->Add(hot);
  }
  else if (cn_env("CLUSTER_OWNQUEEN") == "1")
  {
    TString name = "Q-" + cn_role, title = "harness: mandatory queen of " + cn_role;
    ZQueen* q = new ZQueen(1024, name.Data(), title.Data());
    g_king->Enthrone(q);
    q->SetMandatory(true);
    ZList* top = new ZList("top");  q->CheckIn(top);  q->Add(top);
  }
}

void sc_modify()
{
  if (cn_role == "sun") return;

  ZQueen* sh  = cn_find_queen("Shared");
  ZList*  top = cn_child<ZList>(sh, "top");
  cn_instantiate_into(sh, top, "from-" + cn_role);
  // Every moon overwrites the same title at once; all copies must agree on the winner.
  TString title = "hot-by-" + cn_role;
  cn_post(cn_child<ZNode>(top, "hot")->S_SetTitle(title.Data()));

  TString mq = cn_env("CLUSTER_MASTERQUEEN");
  if (mq != "")
  {
    ZQueen* q = cn_find_queen(mq);
    cn_instantiate_into(q, cn_child<ZList>(q, "top"), "from-" + cn_role);
  }
}

//==============================================================================

void cluster_node()
{
  cn_role   = gSystem->Getenv("CLUSTER_ROLE");
  cn_rundir = gSystem->Getenv("CLUSTER_RUNDIR");

  ASSERT_MACRO(gled_globals);

  if (g_saturn->GetSunAbsolute() == false)
    Gled::theOne->WaitUntilQueensLoaded();

  bool scale = (cn_env("CLUSTER_SCENARIO") == "scale");

  const char* phases[] = { "build", "mirror", "modify", "dump" };
  for (const char* ph : phases)
  {
    TString phase(ph);
    try
    {
      cn_wait_for("go." + phase, 900);
      cn_log("phase " + phase);
      if      (phase == "build")
      {
        if (scale) sc_build(); else cn_build();
        // gled itself allows moons only after the startup macros return.
        if (cn_env("CLUSTER_ALLOW_MOONS") == "1")
          g_saturn->AllowMoons();
      }
      else if (phase == "mirror") { if ( ! scale) cn_mirror(); }
      else if (phase == "modify") { if (scale) sc_modify(); else cn_modify(); }
      else if (phase == "dump")   cn_dump("dump." + cn_role + ".txt");
      cn_touch("done." + cn_role + "." + phase);
    }
    catch (Exc_t& exc)
    {
      cn_log("FAILED in phase " + phase + ": " + exc);
      cn_touch("fail." + cn_role + "." + phase, exc);
      return;
    }
  }
  cn_log("all phases done");
}
