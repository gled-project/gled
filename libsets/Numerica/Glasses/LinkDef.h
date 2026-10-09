// Rules besides those for the glasses; gled_mk_dict_gen.pl merges them into
// dict/Numerica_LinkDef.h.

#pragma link C++ class gled::ODECrawlerMaster+;
#pragma link C++ class gled::ODEStorage+;
#pragma link C++ class gled::ODEStorageT<Float_t>+;
#pragma link C++ class gled::ODEStorageT<Double_t>+;
#pragma link C++ typedef gled::ODEStorageF;
#pragma link C++ typedef gled::ODEStorageD;
#pragma link C++ class gled::WarmAmoebaMaster;
