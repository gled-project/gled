LIB_SET_NAME	  := GledGTS
LIB_SET_ID	  := 16
REQUIRES_LIB_SETS := GledCore Numerica
REQUIRES_EXTERNAL := gts-cxx


BLESSED_DIRS	= Glasses
BASE_DIRS	= Stones GTS

DICT_DIRS	= Glasses Stones
VIEW_DIRS	= FL
VIEW_FL_DIRS	= FL

# gts-cxx comes from an external area (configure --external <dir>).
BASE_LD_LIBS	= -lgts-cxx

#--------------------------------------------------------------
include ${GLEDSYS}/make/make_base.inc
#--------------------------------------------------------------

# gts-cxx needs C++23; ROOT's #warning about the mismatch is silenced. The
# glass headers do not include gts-cxx, so the dictionary stays at C++20.
CXXFLAGS += -std=c++23 -Wno-cpp

#--------------------------------------------------------------
include ${GLEDSYS}/make/make_targets.inc
#--------------------------------------------------------------

clean::
distclean::
