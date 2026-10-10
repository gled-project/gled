##########################################################################
#
# Make-include file for Gled specifying common targets.
#
##########################################################################

include ${GLEDSYS}/make/make_codegen.inc

all_targets:: ${LIBSET_PREREQS}

all_targets:: ${ALL_LIBS}

all_targets:: ${LIBSET_TARGETS}

default: depend
	${MAKE} all_targets
	${MAKE} post_build_install

# The first step of the top-level build: the dictionary and the C++ module of
# the libset, linked into ${GLEDSYS}/lib, where the modules of the libsets
# that require this one find it.
.PHONY: module
module: ${DICT_AUTO_SRCS}
	${MUTE} ${GLED_INSTALL_DIRS} -rs ${GLEDSYS} ${LIBSET_DIR} lib

base_lib: ${BASE_LIB_NAME}
	@echo ${LIB_SET_NAME} > lib/gled_lid_${LIB_SET_ID}

view_lib: ${VIEW_LIB_NAME}

rnr_libs: ${RNR_LIBS}

# Build-install
.PHONY: pre_build_install post_build_install build_install build_uninstall

# The demos of a libset appear in the build area as one directory link,
# ${GLEDSYS}/demos/<LibSet> -> <libset dir>/demos.
LIBSET_DEMOS := $(abspath ${LIBSET_DIR}/demos)
ifneq ($(wildcard ${LIBSET_DEMOS}),)
LINK_DEMOS   := mkdir -p ${GLEDSYS}/demos && ln -sfn ${LIBSET_DEMOS} ${GLEDSYS}/demos/${LIB_SET_NAME}
UNLINK_DEMOS := if [ -L ${GLEDSYS}/demos/${LIB_SET_NAME} ]; then rm -f ${GLEDSYS}/demos/${LIB_SET_NAME}; fi
else
LINK_DEMOS   := true
UNLINK_DEMOS := true
endif

pre_build_install::
	${ECHO} -I- Pre-build-install
	${MUTE} ${GLED_INSTALL_DIRS} -rs ${GLEDSYS} ${LIBSET_DIR} ${PRE_BUILD_INSTALL_DIRS}

post_build_install::
	${ECHO} -I- Post-build-install
ifeq (${GLED_SYSTEM},Darwin)
	${MUTE} cd lib; ${GLEDSYS}/libexec/gled_liblink.sh .dylib .so
endif
	${MUTE} ${GLED_INSTALL_DIRS} -rs ${GLEDSYS} ${LIBSET_DIR} ${POST_BUILD_INSTALL_DIRS}
	${MUTE} ${LINK_DEMOS}

build_install::
	${ECHO} -I- Build-install
	${MUTE} ${GLED_INSTALL_DIRS} -rs ${GLEDSYS} ${LIBSET_DIR} ${BUILD_INSTALL_DIRS}
	${MUTE} ${LINK_DEMOS}

build_uninstall::
	${ECHO} -I- Build-uninstall
	${MUTE} ${GLED_UNINSTALL_DIRS} -rs ${GLEDSYS} ${LIBSET_DIR} ${BUILD_INSTALL_DIRS}
	${MUTE} ${UNLINK_DEMOS}

# Blob install
.PHONY: blob_install blob_uninstall

blob_install::
	${ECHO} -I- Blob-install
	${MUTE} ${GLED_INSTALL_DIRS} -r ${GLED_BLOB_DIR} ${LIBSET_DIR} ${BLOB_INSTALL_DIRS}

blob_uninstall::
	${ECHO} -I- Blob-uninstall
	${MUTE} ${GLED_UNINSTALL_DIRS} -r ${GLED_BLOB_DIR} ${LIBSET_DIR} ${BLOB_INSTALL_DIRS}

# Install
ifneq (${GLED_PREFIX},<none>)
.PHONY: install uninstall

install::	
	${ECHO} -I- Install
	${MUTE} ${GLED_INSTALL_FILES} -r ${GLED_BIN_DIR}  ${LIBSET_DIR}/bin
	${MUTE} ${GLED_INSTALL_FILES} -r ${GLED_LIB_DIR}  ${LIBSET_DIR}/lib
	${MUTE} ${GLED_INSTALL_DIRS}  -r ${GLED_DATA_DIR} ${LIBSET_DIR} ${INSTALL_DATA_DIRS}

uninstall::
	${ECHO} -I- Uninstall
	${MUTE} ${GLED_UNINSTALL_FILES} -r ${GLED_BIN_DIR}  ${LIBSET_DIR}/bin
	${MUTE} ${GLED_UNINSTALL_FILES} -r ${GLED_LIB_DIR}  ${LIBSET_DIR}/lib
	${MUTE} ${GLED_UNINSTALL_DIRS}  -r ${GLED_DATA_DIR} ${LIBSET_DIR} ${INSTALL_DATA_DIRS}
endif

# like install ... only into a tar file (also Readme_<libset> is added)
# make sure to set environment variable BIN_DIST_TAR
bin_dist:
	@perl -e "die \"must set BIN_DIST_TAR environment variable for target bin_dist\" if \"${BIN_DIST_TAR}\" eq \"\""
	if [ ! -e Readme_${LIB_SET_NAME} ]; then touch Readme_${LIB_SET_NAME}; fi
	tar cf ${BIN_DIST_TAR} Readme_${LIB_SET_NAME} ${INSTALL_DIRS}

# Depend and pre-depend group
.PHONY:	auto_dirs glue manifest pre_depend libset_depend depend

auto_dirs:
	${MUTE} mkdir -p ${GLUE_DIR} ${DICT_DIR} ${VIEW_DIR}

glue: ${GLUE_FILES}

manifest: ${MANIFEST}

# The glue and make_dict.inc are also remade by the rules in
# make_codegen.inc; pre_depend is needed after adding or removing headers.
pre_depend:: auto_dirs glue manifest
	${ECHO} -d- Generating ROOT dictionary dependencies
	${MUTE} ${MKDICTDEP} ${LIB_SET_NAME} ${DICT_HDRS} > make_dict.inc


p7_tgts:   ${BLESSED_H7} ${P7_AUTO_HDRS} ${VIEW_SECS}

dep_tgts:  p7_tgts ${MANIFEST} ${ALL_DEPS} ${DEPEND_TGTS}

depend:: dep_tgts
	@touch .depend_done

ifdef DEPEND_DONE
ifeq (${QUIET_GOALS},)
-include ${ALL_DEPS}
endif
endif

# Cleaners
.PHONY:	dist clean distclean

dist:
	(cd ..; tar czf ${LIB_SET_NAME}-`date +"%y%m%d-%H%M"`.tgz ${LIB_SET_NAME})

undepend:
	${MUTE} find . -name \*.d | xargs rm -f

clean::
	${MUTE} rm -f ${ALL_OBJS} ${ALL_DEPS} .depend_done
	${MUTE} find . -type f \( -name '*.d' -or -name '*.o' \) | xargs rm -f

distclean::
	${MAKE} build_uninstall
	${MAKE} clean
	${MUTE} rm -rf ${DICT_DIR} ${VIEW_DIR} ${GLUE_DIR}
	${MUTE} rm -rf ${VIEW_FL_SECS} ${BLESSED_H7} ${BLESSED_C7}
	${MUTE} rm -rf lib/gled_lid_${LIB_SET_ID} lib/lib* lib/*.pcm \
		lib/module.modulemap module.modulemap ${MANIFEST} make_dict.inc
#	some .h7, .c7 may still be lurking around ... ditch them
	${MUTE} find . -name '*.h7' -or -name '*.c7' | xargs rm -f

##########################################################################
# echo group ... to be used w/ grep, perl, xargs ...
##########################################################################

echo_requires_libsets:
	@echo ${REQUIRES_LIB_SETS}
echo_libset_id:
	@echo ${LIB_SET_ID}

echo_cppflags:
	@echo ${CPPFLAGS}
echo_cxxflags:
	@echo ${CXXFLAGS}
echo_ldflags:
	@echo ${LDFLAGS}
echo_ldsharedopt:
	@echo ${LDSHAREDOPT}
echo_ldext:
	@echo ${LDEXT}

echo_bless_hdrs:
	@echo ${BLESSED_HDRS}
echo_bless_srcs:
	@echo ${BLESSED_SRCS}
echo_bless_h7:
	@echo ${BLESSED_H7}
echo_bless_c7:
	@echo ${BLESSED_C7}

echo_baseh:
	@echo ${BASE_HDRS}
echo_bases:
	@echo ${BASE_SRCS}

echo_dict:
	@echo Dict headers:
	@echo ${DICT_HDRS}
	@echo Dict sources:
	@echo ${DICT_SRCS}

echo_dict_dirs:
	@echo ${DICT_DIRS}

echo_fl_fls:
	@echo ${VIEW_FL_FLS}
echo_fl_secs:
	@echo ${VIEW_FL_SECS}

echo_all_stuff:
	@echo All sources:
	@echo ${ALL_SRCS}
	@echo All headers:	
	@echo ${ALL_HDRS}
	@echo All objects:	
	@echo ${ALL_OBJS}
	@echo All libraries:	
	@echo ${ALL_LIBS}
	@echo View secondary files:	
	@echo ${VIEW_SECS}
	@echo Libset targets:
	@echo ${LIBSET_TARGETS}
