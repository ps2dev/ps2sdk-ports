
lib_LTLIBRARIES = libconfig.la

if BUILDCXX
lib_LTLIBRARIES += libconfig++.la
endif

# Versioning rules ( C:R:A )
#
# 1. Start with version 0:0:0.
# 2. If any of the sources have changed, increment R. This is a new revision
#    of the current interface.
# 3. If the interface has changed, increment C and set R to 0. This is the
#    first revision of a new interface.
# 4. If the new interface is a superset of the previous interface
#    (that is, if the previous interface has not been broken by the
#    changes in this new release), increment A. This release is backwards
#    compatible with the previous release.
# 5. If the new interface has removed elements with respect to the
#    previous interface, then backward compatibility is broken; set A to 0.
#    This release has a new, but backwards incompatible interface.
#
# For more info see section 6.3 of the GNU Libtool Manual.

VERINFO = -version-info 8:0:0

PARSER_PREFIX = libconfig_yy

libconfig_la_LDFLAGS = $(VERINFO) -no-undefined

libconfig___la_LDFLAGS = $(VERINFO) -no-undefined

libsrc = libconfig.c scanner.l grammar.y private.h wincompat.h
libinc = libconfig.h

libsrc_cpp =  $(libsrc) libconfigcpp.c++
libinc_cpp =  $(libinc) libconfig.h++

BUILT_SOURCES = scanner.c scanner.h grammar.c grammar.h

libconfig_la_SOURCES = $(libsrc)

libconfig___la_SOURCES = $(libsrc_cpp)

libcppflags = -D_REENTRANT

if GNU_WIN
libcppflags += -DLIBCONFIG_EXPORTS
endif

libconfig_la_CPPFLAGS = -DPARSER_PREFIX=\"$(PARSER_PREFIX)\" $(libcppflags)
libconfig___la_CPPFLAGS = -DPARSER_PREFIX=\"$(PARSER_PREFIX)\" $(libcppflags)

include_HEADERS = $(libinc)

if BUILDCXX
include_HEADERS += $(libinc_cpp)
endif

AM_YFLAGS = -d -p $(PARSER_PREFIX)

AM_LFLAGS = --header-file=scanner.h --prefix=$(PARSER_PREFIX)

EXTRA_DIST = \
	$(BUILT_SOURCES) \
	libconfig++.vcproj \
	libconfig++_stub.vcproj \
	libconfig.vcproj \
	libconfig_stub.vcproj \
	libconfig.sln \
	test.cfg \
	TODO \
	debian/changelog \
	debian/compat \
	debian/control \
	debian/copyright \
	debian/docs \
	debian/libconfig++8-dev.install \
	debian/libconfig++8.install \
	debian/libconfig8-dev.docs \
	debian/libconfig8-dev.install \
	debian/libconfig8.info \
	debian/libconfig8.install \
	debian/rules \
	debian/shlibs \
	debian/watch \
	libconfig.spec \
	libconfigcpp.cc libconfig.hh

pkgconfigdir = $(libdir)/pkgconfig

pkgconfig_DATA = libconfig.pc

if BUILDCXX
pkgconfig_DATA += libconfig++.pc
endif

SUBDIRS = . samples doc

.PHONY: dist-rpm

dist-rpm: distcheck
	rpmbuild -ta $(distdir).tar.gz


msvc7:
	find . -name '*\.vcproj' | grep -v '7' \
	| while read x; do \
		d=`dirname $$x`; \
		y=`basename $$x .vcproj`-msvc7.vcproj; \
		rm -f $$d/$$y; \
		perl -p -e 's/Version="8.00"/Version="7.10"/g' < $$x > $$d/$$y; \
	done; \
	rm -f libconfig-msvc7.sln libconfig-msvc7.sln.tmp; \
	perl -p -e 's/Version 9.00/Version 8.00/g' < libconfig.sln \
		> libconfig-msvc7.sln.tmp; \
	perl -p -e 's/\.vcproj/-msvc7.vcproj/g' \
		< libconfig-msvc7.sln.tmp > libconfig-msvc7.sln; \
	rm -f libconfig-msvc7.sln.tmp
