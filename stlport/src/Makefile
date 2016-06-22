#
# Note : this makefile is for gcc-2.95 and later !
#

#
# compiler
#
CC = $(EE_CC)
CXX = $(EE_CXX) # -fexceptions

#
# Basename for libraries
#
LIB_BASENAME = libstlport

#
# guts for common stuff
#
#
LINK=$(EE_AR) cru
# 2.95 flag
DYN_LINK=$(EE_CXX) -pthread -fexceptions -shared -o

OBJEXT=o
DYNEXT=so
STEXT=a
RM=rm -rf
PATH_SEP=/
MKDIR=mkdir -p
COMP=GCC$(ARCH)
INSTALL_STEP = install_unix 

all: all_static

include common_macros.mak

WARNING_FLAGS= -Wall -W -Wno-sign-compare -Wno-unused -Wno-uninitialized
#-ftemplate-depth-32 -frtti

CXXFLAGS_COMMON = -I${STLPORT_DIR} ${WARNING_FLAGS} -I${PS2SDK}/common/include -I${PS2SDK}/ee/include -I${PS2DEV}/ee/include/c++/3.2.3 -I${PS2DEV}/ee/include/c++/3.2.3/ee

CXXFLAGS_RELEASE_static = $(CXXFLAGS_COMMON) -O2

CXXFLAGS_DEBUG_static = $(CXXFLAGS_COMMON)

CXXFLAGS_STLDEBUG_static = $(CXXFLAGS_DEBUG_static) -D_STLP_DEBUG
CXXFLAGS_STLDEBUG_dynamic = $(CXXFLAGS_DEBUG_dynamic) -D_STLP_DEBUG

include common_percent_rules.mak
include common_rules.mak
include $(PS2SDK)/Defs.make
include ../../Makefile.eeglobal

#install: all
#	cp -p $(LIB_TARGET) ${D_LIB_TARGET} ../lib

#%.s: %.cpp
#	$(CXX) $(CXXFLAGS) -O4 -S -pto $<  -o $@


