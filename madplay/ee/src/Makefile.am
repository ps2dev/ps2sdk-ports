##
## madplay - MPEG audio decoder and player
## Copyright (C) 2000-2004 Robert Leslie
##
## This program is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; either version 2 of the License, or
## (at your option) any later version.
##
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with this program; if not, write to the Free Software
## Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
##
## If you would like to negotiate alternate licensing terms, you may do
## so by contacting the author: Robert Leslie <rob@mars.org>
##
## $Id$
##

## Process this file with automake to produce Makefile.in

SUBDIRS =		intl po
DIST_SUBDIRS =		intl po m4 msvc++

ACLOCAL_AMFLAGS =	-I m4

bin_PROGRAMS =		madplay
bin_SCRIPTS =		abxtest
EXTRA_PROGRAMS =	madtime madmix mad123 madtag

man_MANS =		madplay.1 abxtest.1
EXTRA_MANS =		madtag.1

EXTRA_DIST =		config.rpath  \
			$(man_MANS) $(EXTRA_MANS) $(bin_SCRIPTS)  \
			CHANGES COPYRIGHT CREDITS README TODO VERSION

default_audio =		audio.c audio.h  \
			audio_aiff.c audio_cdda.c audio_hex.c audio_null.c  \
			audio_raw.c audio_snd.c audio_wave.c
extra_audio =		audio_alsa.c audio_carbon.c audio_empeg.c  \
			audio_esd.c audio_jaguar.c audio_nas.c audio_oss.c  \
			audio_qnx.c audio_sun.c audio_win32.c

getopt_sources =	getopt.c getopt.h getopt1.c

madplay_SOURCES =	global.h madplay.c $(getopt_sources) gettext.h  \
			version.c resample.c filter.c tag.c crc.c rgain.c  \
			player.c  \
			version.h resample.h filter.h tag.h crc.h rgain.h  \
			player.h  \
			$(default_audio)
EXTRA_madplay_SOURCES =	$(extra_audio)
madplay_DEPENDENCIES =	$(depend_audio) $(DEPENDENCIES)
madplay_LDADD =		$(ldadd_audio)  $(LDADD)        $(ldadd_libid3tag)

madtime_SOURCES =	global.h madtime.c gettext.h

mad123_SOURCES =	global.h mad123.c $(getopt_sources)

madmix_SOURCES =	global.h madmix.c gettext.h $(default_audio)
EXTRA_madmix_SOURCES =	$(extra_audio)
madmix_DEPENDENCIES =	$(depend_audio) $(DEPENDENCIES)
madmix_LDADD =		$(ldadd_audio)  $(LDADD)

madtag_SOURCES =	global.h madtag.c $(getopt_sources) gettext.h  \
			tagger.c  \
			tagger.h
madtag_DEPENDENCIES =	$(depend)
madtag_LDADD =		$(ldadd) $(ldadd_libid3tag)

localedir =		$(datadir)/locale
INCLUDES =		-Iintl -DLOCALEDIR=\"$(localedir)\"

depend_audio =		@depend_audio@
ldadd_audio =		$(depend_audio) @ldadd_audio@

ldadd_libmad =		-lmad
ldadd_libid3tag =	-lid3tag

depend =		@LIBOBJS@
ldadd =			$(depend) @LTLIBINTL@

DEPENDENCIES =		$(depend)
LDADD =			$(ldadd)  $(ldadd_libmad)

CLEANFILES =		$(EXTRA_PROGRAMS) *.exe gmon.out

libtool: $(LIBTOOL_DEPS)
	$(SHELL) ./config.status --recheck

profile: madplay gmon.out
	gprof madplay | less

gmon.out: madplay test/profile.mpg
	./madplay -vo pcm:/dev/null test/profile.mpg || rm -f gmon.out

.c.s:
	$(COMPILE) -S $<

again:
	$(MAKE) clean
	$(MAKE)

.PHONY: profile again
