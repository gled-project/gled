CXX		= g++
CC		= gcc
LD		= g++
CPPFLAGS	= $(shell make -s -C ${GLEDSYS}/Geom1 echo_cppflags)
CXXFLAGS	= $(shell make -s -C ${GLEDSYS}/Geom1 echo_cxxflags)
LDFLAGS		= $(shell make -s -C ${GLEDSYS}/Geom1 echo_ldsharedopt)
LDEXT           = $(shell make -s -C ${GLEDSYS}/Geom1 echo_ldext)

SRCS		= $(wildcard *.cxx)
TGTLIBS		= $(SRCS:.cxx=.${LDEXT})

.SUFFIXES: .cxx .cc .h .o


%.o: %.cc
	${CXX} ${CPPFLAGS} -c -o $@ ${CXXFLAGS} $<
%.o: %.cxx
	${CXX} ${CPPFLAGS} -c -o $@ ${CXXFLAGS} $<

%_Dict.cc %_Dict.h: %.h %_LinkDef.h
	@echo Generating dictionary for $*
	rootcint -f $@ -c $*.h $*_LinkDef.h

%.${LDEXT}: %.o # %_Dict.o
	${LD} ${LDFLAGS} -o $@ $^


all: ${TGTLIBS}

clean:
	rm -f *_Dict* *.o *.${LDEXT}
