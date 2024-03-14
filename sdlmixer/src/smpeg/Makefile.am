
## Makefile.am for the smpeg library

bin_SCRIPTS = smpeg-config

CCLD = $(CXXLD)

# The smpeg library target
lib_LTLIBRARIES = libsmpeg.la

libsmpeg_la_SOURCES =		\
	MPEG.cpp		\
	MPEGring.cpp		\
	MPEGlist.cpp		\
	MPEGstream.cpp		\
	MPEGsystem.cpp		\
	MPEGfilter.c		\
	smpeg.cpp		\
	audio/MPEGaudio.cpp	\
	audio/bitwindow.cpp	\
	audio/filter.cpp	\
	audio/filter_2.cpp	\
	audio/hufftable.cpp	\
	audio/mpeglayer1.cpp	\
	audio/mpeglayer2.cpp	\
	audio/mpeglayer3.cpp	\
	audio/mpegtable.cpp	\
	audio/mpegtoraw.cpp	\
	video/MPEGvideo.cpp	\
	video/decoders.cpp	\
	video/decoders.h	\
	video/dither.h		\
	video/floatdct.cpp	\
	video/gdith.cpp		\
	video/jrevdct.cpp	\
	video/motionvec.cpp	\
	video/parseblock.cpp	\
	video/proto.h		\
	video/readfile.cpp	\
	video/util.cpp		\
	video/util.h		\
	video/video.cpp		\
	video/video.h		\
	video/vhar128.cpp	\
	video/vhar128.h		\
	video/mmxflags_asm.S	\
	video/mmxidct_asm.S

libsmpegincludedir = $(includedir)/smpeg
libsmpeginclude_HEADERS =	\
	MPEG.h			\
	MPEGaction.h		\
	MPEGaudio.h		\
	MPEGerror.h		\
	MPEGfilter.h		\
	MPEGring.h		\
	MPEGlist.h		\
	MPEGstream.h		\
	MPEGsystem.h		\
	MPEGvideo.h		\
	smpeg.h

libsmpeg_la_LDFLAGS = 		\
        -release $(LT_RELEASE)	\
	-version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE)

EXTRA_DIST =			\
	CHANGES			\
	COPYING			\
	TODO			\
	README			\
	README.SDL_mixer 	\
	plaympeg.1		\
	gtv.1			\
	VisualC.zip		\
	smpeg.m4		\
	smpeg.spec		\
	smpeg.spec.in		\
	gcc-fat.sh		\
	acinclude		\
	autogen.sh		\
	audio/AUTHORS		\
	audio/COPYING.LIB	\
	audio/README		\
	audio/README.LIB	\
	video/README		\
	video/COPYRIGHT


video/16bit_mmx.lo: video/16bit_mmx.cpp
	$(CXX) -c -o $@ $^ $(CXXFLAGS)
video/32bit_mmx.lo: video/32bit_mmx.cpp
	$(CXX) -c -o $@ $^ $(CXXFLAGS)

# Sample MPEG players
if HAVE_GTK
GTK_PLAYER = gtv
else
GTK_PLAYER =
endif
if HAVE_OPENGL
OPENGL_PLAYER = glmovie
else
OPENGL_PLAYER =
endif
bin_PROGRAMS = plaympeg $(GTK_PLAYER) $(OPENGL_PLAYER)

# Sources for plaympeg
man_MANS = plaympeg.1 gtv.1
plaympeg_SOURCES = plaympeg.c
plaympeg_LDADD = libsmpeg.la

# Sources for gtv
gtv_SOURCES = gtv.c gtv.h
gtv_LDADD = @GTK_LIBS@ libsmpeg.la

# Sources for glmovie
glmovie_SOURCES = glmovie-tile.c glmovie.c glmovie.h
glmovie_LDADD = @GL_LIBS@ libsmpeg.la

# M4 macro file for inclusion with autoconf
m4datadir = $(datadir)/aclocal
m4data_DATA = smpeg.m4

# Rule to build tar-gzipped distribution package
$(PACKAGE)-$(VERSION).tar.gz: dist

# Rule to build RPM distribution package
rpm: $(PACKAGE)-$(VERSION).tar.gz
	cp $(PACKAGE)-$(VERSION).tar.gz /usr/src/redhat/SOURCES
	rpm -ba smpeg.spec

