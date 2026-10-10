LIB_SET_NAME	  := GledCore
LIB_SET_ID	  := 1
REQUIRES_LIB_SETS :=
REQUIRES_EXTERNAL := root fltk

# All this dirs expanded in make_base.inc into <type>_SRCS and _HDRS
# Can add your own before ... or after (are += -ed into the vars)

BLESSED_DIRS	= Glasses
BASE_DIRS	= Ephra Stones Gled Eye RnrBase
DICT_DIRS	= Glasses Stones Gled Ephra

VIEW_DIRS	= GledView FL
VIEW_FL_DIRS	= FL

BASE_LD_LIBS	= ${ROOT_LD_DIR} -lHist -lRIO -lNet -lMatrix -lcrypto

MAINSRCS := saturn.cxx gled.cxx
MAINOBJS := $(MAINSRCS:.cxx=.o)
MAINEXES := bin/saturn bin/gled

PUPILS_LIB      = lib/libGledCore_Pupils.${LDEXT}
PUPILS_HDRS    := $(wildcard Pupils/*.h)
PUPILS_SRCS    := $(wildcard Pupils/*.cxx)
PUPILS_OBJS    := $(addsuffix .o ,$(basename ${PUPILS_SRCS}))

#--------------------------------------------------------------
include ${GLEDSYS}/make/make_base.inc
#--------------------------------------------------------------

# The main programs get header dependencies and clean like the libraries do.
ALL_SRCS += ${MAINSRCS}
ALL_OBJS += ${MAINOBJS}
ALL_DEPS += $(MAINOBJS:.o=.d)


LIBSET_TARGETS += bin/saturn

ifndef SKIP_VIEW_LIBS
  LIBSET_TARGETS   += bin/gled
  ifndef SKIP_RNR_LIBS
    ALL_SRCS       += ${PUPILS_SRCS}
    ALL_HDRS       += ${PUPILS_HDRS}
    ALL_OBJS       += ${PUPILS_OBJS}
    ALL_DEPS       += $(PUPILS_OBJS:.o=.d)
    LIBSET_TARGETS += ${PUPILS_LIB}
  endif
endif

bin/saturn: saturn.o ${BASE_LIB_NAME}
	${ECHO} -X- Linking $@
	${MUTE} ${LD} ${LDFLAGS} saturn.o -o $@ \
	  -Llib -l${LIB_SET_NAME} ${LDFLAGS_ASNEEDED} ${ROOTLIBS} ${SYSLIBS}

bin/gled: gled.o ${BASE_LIB_NAME} ${VIEW_LIB_NAME}
	${ECHO} -X- Linking $@
	${MUTE} ${LD} ${LDFLAGS} gled.o -o $@ \
	  -Llib -l${LIB_SET_NAME} -l${LIB_SET_NAME}_View \
	  ${LDFLAGS_ASNEEDED} ${ROOTLIBS} ${SYSLIBS} ${GUILIBS} ${ROOTGLIBS}
# On 10.7, this creates folder gled.app; and it seems broken.
# ifeq (${GLED_SYSTEM},Darwin)
#	${FLTK_CONFIG} --post $@
# endif


${PUPILS_LIB}: ${PUPILS_OBJS} lib/libGledCore_Rnr_GL.${LDEXT}
	${ECHO} -X- Linking $@
	${MUTE} ${LD} ${LDSHAREDOPT} ${LDFLAGS} ${PUPILS_OBJS} -o $@ \
	  ${ROOT_LD_DIR} ${FLTKLIBS} ${GLLIBS} -Llib -l${LIB_SET_NAME}_Rnr_GL \
	  ${LDFLAGS_ASNEEDED} ${RNR_OWN_LIBS} ${ROOTLIBS}

#--------------------------------------------------------------
include ${GLEDSYS}/make/make_targets.inc
#--------------------------------------------------------------

clean::
	@rm -f ${MAINOBJS}

distclean::
	@rm -f ${MAINEXES}

#===============================================================================

# FTGL is compiled into the GL renderer library (Rnr/GL/FTGL.{h,cxx}).
ABOVE_RNR_GL_LIBS += $(shell pkg-config --libs freetype2)

Rnr/GL/FTGL.o Rnr/GL/GLTextNS.o Rnr/GL/Text_GL_Rnr.o Rnr/GL/WGlFrameStyle_GL_Rnr.o \
Rnr/GL/ZRlFont_GL_Rnr.o : CPPFLAGS += $(shell pkg-config --cflags freetype2)
