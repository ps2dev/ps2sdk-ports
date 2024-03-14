EE_LIB_DIR = ../lib/
EE_LIB = $(EE_LIB_DIR)libsdlmixer.a

EE_CFLAGS   = -DMOD_MUSIC -DWAV_MUSIC -DUSE_RWOPS
EE_CXXFLAGS = -DNOCONTROLS

HAVE_MP3_MUSIC = 1
PS2_ROMFS = 0

MIKMOD_OBJS  = mikmod/mplayer.o mikmod/mdriver.o mikmod/mloader.o
MIKMOD_OBJS += mikmod/load_it.o mikmod/load_s3m.o mikmod/load_mod.o mikmod/load_xm.o 
MIKMOD_OBJS += mikmod/virtch.o mikmod/virtch2.o mikmod/munitrk.o mikmod/mmerror.o
MIKMOD_OBJS += mikmod/mmalloc.o mikmod/sloader.o mikmod/mwav.o
MIKMOD_OBJS += mikmod/drv_nos.o mikmod/drv_sdl.o mikmod/mmio.o
MIKMOD_OBJS += mikmod/mlutil.o mikmod/virtch_common.o mikmod/npertab.o
MIKMOD_OBJS += mikmod/mlreg.o mikmod/mdreg.o

MIXER_OBJS  = dynamic_flac.o dynamic_fluidsynth.o dynamic_mod.o 
MIXER_OBJS += effect_position.o effect_stereoreverse.o effects_internal.o 

ifeq ($(HAVE_MP3_MUSIC), 1)
EE_CFLAGS  += -DMP3_MUSIC
SMPEG_OBJS  = smpeg/smpeg.o smpeg/MPEGlist.o smpeg/MPEGring.o
SMPEG_OBJS += smpeg/audio/bitwindow.o smpeg/audio/filter.o
SMPEG_OBJS += smpeg/audio/filter_2.o smpeg/audio/hufftable.o
SMPEG_OBJS += smpeg/audio/MPEGaudio.o smpeg/audio/mpeglayer1.o
SMPEG_OBJS += smpeg/audio/mpeglayer2.o smpeg/audio/mpeglayer3.o
SMPEG_OBJS += smpeg/audio/mpegtable.o smpeg/audio/mpegtoraw.o
SMPEG_OBJS += smpeg/video/decoders.o smpeg/video/video.o
SMPEG_OBJS += smpeg/video/util.o smpeg/video/motionvec.o
SMPEG_OBJS += smpeg/video/readfile.o smpeg/video/MPEGvideo.o
SMPEG_OBJS += smpeg/video/floatdct.o smpeg/video/jrevdct.o
SMPEG_OBJS += smpeg/video/vhar128.o smpeg/video/gdith.o
SMPEG_OBJS += smpeg/video/parseblock.o smpeg/MPEGfilter.o
SMPEG_OBJS += smpeg/MPEGstream.o smpeg/MPEGsystem.o smpeg/MPEG.o
MIXER_OBJS += dynamic_mp3.o
else
SMPEG_OBJS =
endif

MIXER_OBJS  += dynamic_ogg.o mixer.o
MIXER_OBJS  += load_aiff.o load_flac.o load_ogg.o load_voc.o 
MIXER_OBJS  += music_cmd.o music_flac.o music_mad.o music_mod.o music_modplug.o music_ogg.o music.o wavestream.o

EE_OBJS = $(MIXER_OBJS) $(MIKMOD_OBJS) $(SMPEG_OBJS)

EE_INCS  = -I. -I../include -Imikmod -Ismpeg -I$(PS2SDK)/common/include 
EE_INCS += -I$(PS2SDK)/ports/include -I$(PS2SDK)/ports/include/SDL
EE_LIBS  = 

all: $(EE_LIB)

install: all
	mkdir -p $(DESTDIR)$(PS2SDK)/ports/lib
	mkdir -p $(DESTDIR)$(PS2SDK)/ports/include/SDL
	cp -f $(EE_LIB) $(DESTDIR)$(PS2SDK)/ports/lib
	cp -f SDL_mixer.h $(DESTDIR)$(PS2SDK)/ports/include/SDL

clean:
	rm -f -r $(EE_LIB_DIR) $(EE_OBJS)

include $(PS2SDK)/samples/Makefile.pref
include $(PS2SDK)/samples/Makefile.eeglobal
