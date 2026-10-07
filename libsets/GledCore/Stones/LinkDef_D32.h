// Selection rules for the GledCore_Stones_D32 dictionary.
//
// Templates instantiated with Double32_t need a dictionary of their own: in
// one translation unit HTrans<Double32_t> is the same class as
// HTrans<Double_t>, and rootcling keeps only one of the two rules. ROOT
// splits its own Double32_t instantiations out the same way.

#pragma link C++ class gled::HPoint<Double32_t>+;
#pragma link C++ class gled::HTrans<Double32_t>+;
