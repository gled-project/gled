#ifndef RT_FUNCTIONS_H
#define RT_FUNCTIONS_H

#include <Rtypes.h>

namespace gled
{
class ZNode;
class Eventor;
class RndSMorphCreator;
}

extern gled::ZNode**           subnodes;
extern gled::Eventor*          eventor;
extern gled::RndSMorphCreator* smorph_creator;
extern gled::ZNode*            top_node;

extern void create_smorphs(Int_t NSN = 32, Int_t NNN = 1024, Int_t SMS = 0);

#endif
