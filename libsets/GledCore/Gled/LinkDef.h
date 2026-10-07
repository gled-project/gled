// Selection rules for the GledCore_Gled dictionary.
//
// gled_mk_dict_gen.pl copies this file into dict/GledCore_Gled_LinkDef.h.
//
// Linking of nested classes and nested typedefs is enabled for all classes.

#pragma link C++ class gled::GCondition+;
#pragma link C++ class gled::GKeyRSA+;
#pragma link C++ class gled::Gled+;
#pragma link C++ global gled::Gled::theOne;
#pragma link C++ namespace gled::GledNS+;
#pragma link C++ function gled::GForm;
#pragma link C++ class gled::Exc_t+;
#pragma link C++ class gled::GMutex+;
#pragma link C++ class gled::GMutexHolderBase;
#pragma link C++ class gled::GMutexHolder;
#pragma link C++ class gled::GMutexAntiHolder;
#pragma link C++ class gled::GLensReadHolder;
#pragma link C++ class gled::GLensWriteHolder;
#pragma link C++ class gled::GSelector+;
#pragma link C++ class gled::GSpinLock+;
#pragma link C++ class gled::GThread+;
#pragma link C++ class gled::GTime+;
#pragma link C++ class gled::TRootXTReq+;
#pragma link C++ class gled::XTReqCanvas+;
