// Selection rules for the Numerica_Glasses dictionary, in addition to the glasses.
//
// gled_mk_dict_gen.pl copies this file into dict/Numerica_Glasses_LinkDef.h and adds
// 'class gled::<Glass>+' and 'class gled::ZLink<gled::<Glass>>' for every
// glass in this directory. A rule here naming the glass itself, e.g.
// 'class gled::AList-', replaces the default '+' rule.
//
// Linking of nested classes and nested typedefs is enabled for all classes.

#pragma link C++ class gled::ODECrawlerMaster+;
#pragma link C++ class gled::ODEStorage+;
#pragma link C++ class gled::ODEStorageT<Float_t>+;
#pragma link C++ class gled::ODEStorageT<Double_t>+;
#pragma link C++ typedef gled::ODEStorageF;
#pragma link C++ typedef gled::ODEStorageD;
#pragma link C++ class gled::WarmAmoebaMaster;
