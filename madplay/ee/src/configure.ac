dnl -*- m4 -*-
dnl
dnl madplay - MPEG audio decoder and player
dnl Copyright (C) 2000-2004 Robert Leslie
dnl
dnl This program is free software; you can redistribute it and/or modify
dnl it under the terms of the GNU General Public License as published by
dnl the Free Software Foundation; either version 2 of the License, or
dnl (at your option) any later version.
dnl
dnl This program is distributed in the hope that it will be useful,
dnl but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
dnl GNU General Public License for more details.
dnl
dnl You should have received a copy of the GNU General Public License
dnl along with this program; if not, write to the Free Software
dnl Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
dnl
AC_REVISION([$Id$])dnl

dnl Process this file with autoconf to produce a configure script.

AC_INIT([MPEG audio decoder and player], [0.15.2b], [rob@mars.org], [madplay])
AC_PREREQ(2.53)

AC_CONFIG_SRCDIR([madplay.c])

AM_INIT_AUTOMAKE

AM_CONFIG_HEADER([config.h])

dnl System type.

AC_CANONICAL_HOST

dnl Checks for programs.

# Work-around for autoconf 2.53 bug
case "$host" in
    *-apple-darwin*)
	: ${CPP="cc -E -traditional-cpp"}
	;;
esac

AC_PROG_CC

if test "$GCC" = yes
then
    case "$host" in
	*-*-mingw*)
	    case "$build" in
		*-*-cygwin*)
		    CPPFLAGS="$CPPFLAGS -mno-cygwin"
		    LDFLAGS="$LDFLAGS -mno-cygwin"
		    ;;
	    esac
    esac
fi

dnl Support for libtool.

AC_DISABLE_SHARED
AC_PROG_LIBTOOL

AC_SUBST(LIBTOOL_DEPS)

dnl Compiler options.

arch=""
debug=""
optimize=""
profile=""

set -- $CFLAGS
CFLAGS=""

if test "$GCC" = yes
then
    CFLAGS="-Wall"
fi

while test $# -gt 0
do
    case "$1" in
	-Wall)
	    if test "$GCC" = yes
	    then
		:
	    else
		CFLAGS="$CFLAGS $1"
	    fi
	    shift
	    ;;
	-g)
	    debug="-g"
	    shift
	    ;;
	-mno-cygwin)
	    shift
	    ;;
	-m*)
	    arch="$arch $1"
	    shift
	    ;;
	-fomit-frame-pointer)
	    shift
	    ;;
	-O*|-f*)
	    optimize="$1"
	    shift
	    ;;
	*)
	    CFLAGS="$CFLAGS $1"
	    shift
	    ;;
    esac
done

if test "$GCC" = yes
then
    if test -z "$arch"
    then
	case "$host" in
	    i386-*)           ;;
	    i?86-*)           arch="-march=i486" ;;
	    arm*-empeg-*)     arch="-march=armv4 -mtune=strongarm1100" ;;
	    armv4*-*)         arch="-march=armv4 -mtune=strongarm" ;;
	    powerpc-*)        ;;
	    mips*-agenda-*)   arch="-mcpu=vr4100" ;;
	    mips*-luxsonor-*) arch="-mips1 -mcpu=r3000 -Wa,-m4010" ;;
	esac
    fi

    if test -n "$optimize"
    then
	: #? optimize="$optimize -funroll-loops"
	: #? optimize="$optimize -finline-functions"
    fi
fi

dnl Internationalization support.

ALL_LINGUAS="en es fr hr no"

AM_GNU_GETTEXT([use-libtool])
AM_GNU_GETTEXT_VERSION(0.14.1)

dnl Checks for header files.

AC_HEADER_STDC
AC_CHECK_HEADERS(assert.h termios.h stropts.h  \
		 sys/types.h fcntl.h errno.h  \
		 sys/soundcard.h machine/soundcard.h)

AC_CHECK_HEADER(mad.h, [], [
	AC_MSG_ERROR([mad.h was not found
*** You must first install libmad before you can build this package.
*** If libmad is already installed, you may need to use the CPPFLAGS
*** environment variable to specify its installed location, e.g. -I<dir>.])
])

AC_CHECK_HEADER(id3tag.h, [], [
	AC_MSG_ERROR([id3tag.h was not found
*** You must first install libid3tag before you can build this package.
*** If libid3tag is already installed, you may need to use the CPPFLAGS
*** environment variable to specify its installed location, e.g. -I<dir>.])
])

dnl Checks for typedefs, structures, and compiler characteristics.

AC_C_CONST
AC_C_INLINE
AC_C_BIGENDIAN

dnl Checks for library functions.

AC_CHECK_LIB(mad, mad_decoder_run, :, [
	AC_MSG_ERROR([libmad was not found
*** You must first install libmad before you can build this package.
*** If libmad is already installed, you may need to use the LDFLAGS
*** environment variable to specify its installed location, e.g. -L<dir>.])
])

AC_CHECK_LIB(id3tag, id3_tag_parse, :, [
	AC_MSG_ERROR([libid3tag was not found
*** You must first install libid3tag before you can build this package.
*** If libid3tag is already installed, you may need to use the LDFLAGS
*** environment variable to specify its installed location, e.g. -L<dir>.])
], [-lz])

AC_FUNC_VPRINTF
AC_CHECK_FUNCS(madvise localeconv)
AC_REPLACE_FUNCS(strcasecmp strncasecmp)
AC_SEARCH_LIBS(pow, m)
AC_SEARCH_LIBS(log10, m)

case "$host" in
    *-linux*) AC_DEFINE(HAVE_MMAP) ;;
    *)        AC_FUNC_MMAP ;;
esac

AC_MSG_CHECKING([whether to use mmap])
AC_ARG_ENABLE(mmap, AC_HELP_STRING([--disable-mmap],
		    [do not use mmap even if available (lower mem usage)]),
[
    case "$enableval" in
	yes)
	    AC_DEFINE(USE_MMAP, 1,
		[Define if you have mmap() and you want to use it.])
	    ;;
    esac
], [
    AC_DEFINE(USE_MMAP)
])
AC_MSG_RESULT(${enable_mmap-yes})

AC_CACHE_SAVE

dnl Select an audio system.

depend_audio=""
ldadd_audio=""
AC_SUBST(depend_audio)
AC_SUBST(ldadd_audio)

AC_MSG_CHECKING([for audio support])

audio="unknown"

want_alsa=no
AC_ARG_WITH(alsa, AC_HELP_STRING([--with-alsa],
		  [use Advanced Linux Sound Architecture as default]),
[
    case "$withval" in
	yes)
	    want_alsa=yes
	    if test "$audio" = unknown
	    then
		audio="alsa"
	    else
		AC_MSG_ERROR([cannot use both --with-$audio and --with-alsa])
	    fi
	    ;;
    esac
])

want_esd=yes
AC_ARG_WITH(esd, AC_HELP_STRING([--with-esd],
		 [use Enlightened Sound Daemon (EsounD) as default]),
[
    case "$withval" in
	yes)
	    if test "$audio" = unknown
	    then
		audio="esd"
	    else
		AC_MSG_ERROR([cannot use both --with-$audio and --with-esd])
	    fi
	    ;;
	no)
	    want_esd=no
	    ;;
    esac
])

dnl want_nas=yes
dnl AC_ARG_WITH(nas, AC_HELP_STRING([--with-nas],
dnl 		     [use Network Audio System (NAS) as default]),
dnl [
dnl     case "$withval" in
dnl 	yes)
dnl 	    if test "$audio" = unknown
dnl 	    then
dnl 		audio="nas"
dnl 	    else
dnl 		AC_MSG_ERROR([cannot use both --with-$audio and --with-nas])
dnl 	    fi
dnl 	    ;;
dnl 	no)
dnl 	    want_nas=no
dnl 	    ;;
dnl     esac
dnl ])

if test "$audio" = unknown
then
    case "$host" in
	arm*-empeg-*) audio="empeg" ;;
    esac
fi

dnl if test "$audio" = unknown
dnl then
dnl     AC_EGREP_CPP(kAudioUnitProperty_SetRenderCallback, [
dnl # include <AudioUnit/AudioUnit.h>
dnl ], [audio="jaguar";
dnl     ldadd_audio="$ldadd_audio -framework AudioUnit -framework CoreServices"])
dnl fi

if test "$audio" = unknown
then
    AC_EGREP_CPP(SndNewChannel, [
# include <Carbon/Carbon.h>
], [audio="carbon"; ldadd_audio="$ldadd_audio -framework Carbon"])
fi

if test "$audio" = unknown
then
    AC_EGREP_CPP(waveOutOpen, [
# include <mmsystem.h>
], [audio="win32"; ldadd_audio="$ldadd_audio -lwinmm"])
fi

if test "$audio" = unknown
then
    AC_EGREP_CPP(yes, [
# include <sys/soundcard.h>
# if defined(OPEN_SOUND_SYSTEM) || defined(UNIX_SOUND_SYSTEM) ||  \
    defined(VOXWARE)
yes
# endif
], [audio="oss"])
fi

if test "$audio" = unknown
then
    AC_EGREP_CPP(yes, [
# include <machine/soundcard.h>
# if defined(OPEN_SOUND_SYSTEM) || defined(UNIX_SOUND_SYSTEM) ||  \
    defined(VOXWARE)
yes
# endif
], [audio="oss"])
fi

if test "$audio" = unknown
then
    AC_EGREP_CPP(audio_info_t, [
# include <sys/audioio.h>
], [audio="sun"])
fi

if test "$audio" = unknown
then
    AC_EGREP_CPP(yes, [
# include <sys/asoundlib.h>
# if defined(SND_PCM_OPEN_PLAYBACK)
yes
# endif
], [audio="qnx"; ldadd_audio="$ldadd_audio -lasound"])
fi

AC_MSG_RESULT($audio)

if test "$want_alsa" = yes
then
    AC_CHECK_LIB(asound, snd_pcm_open, have_libasound=yes, have_libasound=no)

    if test "$have_libasound" = yes
    then
	AC_DEFINE(HAVE_LIBASOUND, 1,
	    [Define if you have the asound library (-lasound).])
	ldadd_audio="$ldadd_audio -lasound"
    fi
fi

if test "$want_esd" = yes
then
    AC_CHECK_LIB(esd, esd_open_sound, have_libesd=yes, have_libesd=no)

    if test "$have_libesd" = yes
    then
	AC_DEFINE(HAVE_LIBESD, 1,
	    [Define if you have the esd library (-lesd).])
	ldadd_audio="$ldadd_audio -lesd"
    fi
fi

dnl if test "$want_nas" = yes
dnl then
dnl     AC_PATH_X
dnl     if test "$no_x" = yes
dnl     then
dnl 	have_libaudio=no
dnl     else
dnl 	AC_CHECK_LIB(audio, AuOpenServer,
dnl 	    have_libaudio=yes, have_libaudio=no, -L$x_libraries -lXau -lXt)
dnl 
dnl 	if test "$have_libaudio" = yes
dnl 	then
dnl 	    AC_DEFINE(HAVE_LIBAUDIO)
dnl 	    ldadd_audio="$ldadd_audio -laudio -L$x_libraries -lXau -lXt"
dnl 	fi
dnl     fi
dnl fi

if test "$audio" = alsa
then
    if test "$have_libasound" = no
    then
	AC_MSG_ERROR([cannot use --with-alsa because libasound was not found])
    fi
else
    if test "$want_alsa" = yes && test "$have_libasound" = yes
    then
	depend_audio="$depend_audio audio_alsa.o"
    fi
fi

if test "$audio" = esd
then
    if test "$have_libesd" = no
    then
	AC_MSG_ERROR([cannot use --with-esd because libesd was not found])
    fi
else
    if test "$want_esd" = yes && test "$have_libesd" = yes
    then
	depend_audio="$depend_audio audio_esd.o"
    fi
fi

dnl if test "$audio" = nas
dnl then
dnl     if test "$have_libaudio" = no
dnl     then
dnl 	AC_MSG_ERROR([cannot use --with-nas because libaudio was not found])
dnl     fi
dnl else
dnl     if test "$want_nas" = yes && test "$have_libaudio" = yes
dnl     then
dnl 	depend_audio="$depend_audio audio_nas.o"
dnl     fi
dnl fi

if test "$audio" = unknown
then
    AC_MSG_WARN([audio support not available; using null module])
    AC_DEFINE(AUDIO_DEFAULT, audio_null,
	[Define to be the name of your default audio output module.])
else
    AC_DEFINE_UNQUOTED(AUDIO_DEFAULT, [audio_$audio])

    depend_audio="$depend_audio audio_$audio.o"
fi

dnl madplay options...

AC_MSG_CHECKING([whether to enable profiling])
AC_ARG_ENABLE(profiling, AC_HELP_STRING([--enable-profiling],
			 [generate profiling code]),
[
    case "$enableval" in
	yes) profile="-pg" ;;
    esac
])
AC_MSG_RESULT(${enable_profiling-no})

AC_MSG_CHECKING([whether to enable debugging])
AC_ARG_ENABLE(debugging, AC_HELP_STRING([--enable-debugging],
			 [enable diagnostic debugging support])
AC_HELP_STRING([--disable-debugging],
			 [do not enable debugging and use more optimization]),
[
    case "$enableval" in
	yes)
	    AC_DEFINE(DEBUG, 1,
		[Define to enable diagnostic debugging support.])
	    optimize=""
	    ;;
	no)
	    if test -n "$profile"
	    then
    AC_MSG_ERROR([--enable-profiling and --disable-debugging are incompatible])
	    fi

	    AC_DEFINE(NDEBUG, 1,
		[Define to disable debugging assertions.])
	    debug=""
	    if test "$GCC" = yes
	    then
		optimize="$optimize -fomit-frame-pointer"
	    fi
	    ;;
    esac
])
AC_MSG_RESULT(${enable_debugging-default})

case "$enable_debugging" in
    yes) AC_CHECK_LIB(efence, malloc) ;;
esac

AC_MSG_CHECKING([whether to enable experimental code])
AC_ARG_ENABLE(experimental, AC_HELP_STRING([--enable-experimental],
			    [enable experimental code]),
[
    case "$enableval" in
	yes)
	    AC_DEFINE(EXPERIMENTAL, 1, [Define to enable experimental code.])
	    ;;
    esac
])
AC_MSG_RESULT(${enable_experimental-no})

dnl Create output files.

test -n "$arch"     && CFLAGS="$CFLAGS $arch"
test -n "$debug"    && CFLAGS="$CFLAGS $debug"
test -n "$optimize" && CFLAGS="$CFLAGS $optimize"
test -n "$profile"  && CFLAGS="$CFLAGS $profile" LDFLAGS="$LDFLAGS $profile"

AC_CONFIG_FILES([Makefile msvc++/Makefile  \
	intl/Makefile po/Makefile.in m4/Makefile  \
	madplay.list])
AC_OUTPUT
