// Selection rules for the GledCore_Glasses dictionary, in addition to the glasses.
//
// gled_mk_dict_gen.pl copies this file into dict/GledCore_Glasses_LinkDef.h and adds
// 'class gled::<Glass>+' and 'class gled::ZLink<gled::<Glass>>' for every
// glass in this directory. A rule here naming the glass itself, e.g.
// 'class gled::AList-', replaces the default '+' rule.
//
// Linking of nested classes and nested typedefs is enabled for all classes.

#pragma link C++ class gled::AList-;
#pragma link C++ class gled::AList::ElRep+;
#pragma link C++ typedef gled::lpZGlass_t;
#pragma link C++ typedef gled::lpZGlass_i;
#pragma link C++ class gled::ZDeque-;
#pragma link C++ class gled::ZLinkBase-;
#pragma link C++ class gled::ZGlass::NameChangeCB+;
#pragma link C++ class gled::ZGlass::RayAbsorber+;
#pragma link C++ class gled::FID_t+;
#pragma link C++ class gled::FMID_t+;
#pragma link C++ class gled::An_ID_Demangler+;
#pragma link C++ class gled::MIR_Priest+;
#pragma link C++ class gled::ZList-;
#pragma link C++ class gled::ZQueen::LensDetails+;
#pragma link C++ class gled::ZStringMap-;
#pragma link C++ class gled::ZVector-;
