# $Header$
LIB_SET_NAME	:= Tmp1
LIB_SET_ID	:= 128
REQUIRES_LIB_SETS := GledCore Geom1 GledGTS # add

# Directories containing glasses - full Gled enabled classes.
BLESSED_DIRS	:= Glasses
# Directories to be included in base library.
BASE_DIRS	:= Stones
# Directories for which dictionaries will be generates.
DICT_DIRS	:= Glasses Stones
# Directories to go into the View / GUI library.
VIEW_DIRS	:=
# files from /*.fl are processed by fluid
VIEW_FL_DIRS	:=

#--------------------------------------------------------------
include ${GLEDSYS}/make/make_base.inc
#--------------------------------------------------------------

# LIBSET_PREREQS +=
# LIBSET_TARGETS +=

# user targets go here; enumerate them in LIBSET_PREREQS/TARGETS

#--------------------------------------------------------------
include ${GLEDSYS}/make/make_targets.inc
#--------------------------------------------------------------

clean::
distclean::
