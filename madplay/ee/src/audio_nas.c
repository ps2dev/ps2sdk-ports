/*
 * madplay - MPEG audio decoder and player
 * Copyright (C) 2000-2004 Robert Leslie
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * $Id$
 */

/* N.B. this audio module is buggy and unfinished */

# ifdef HAVE_CONFIG_H
#  include "config.h"
# endif

# include "global.h"

# include <audio/audiolib.h>
# include <mad.h>

# include "gettext.h"

# include "audio.h"

static AuServer *server;
static AuFlowID flow;
static audio_pcmfunc_t *audio_pcm;

static struct {
  unsigned char *data;
  unsigned int len;
} buffer;

static
int init(struct audio_init *init)
{
  char const *servername;
  char *message;

  servername = init->path;
  if (servername && *servername == 0)
    servername = 0;

  server = AuOpenServer(servername, 0, 0, 0, 0, &message);
  if (server == 0) {
    audio_error = message ? message : _("AuOpenServer() failed");
    /* N.B. AuFree(message) never called */
    return -1;
  }

  flow = AuCreateFlow(server, 0);
  if (flow == 0) {
    AuCloseServer(server);
    audio_error = _("could not create flow");
    return -1;
  }

  return 0;
}

static
AuDeviceID getdevice(AuServer const *server, unsigned int *channels)
{
  AuDeviceID device = AuNone;
  int i;

  for (i = 0; i < AuServerNumDevices(server); ++i) {
    if (AuDeviceKind(AuServerDevice(server, i)) ==
	AuComponentKindPhysicalOutput &&
	AuDeviceNumTracks(AuServerDevice(server, i)) == *channels) {
      device = AuDeviceIdentifier(AuServerDevice(server, i));
      break;
    }
  }

  if (device == AuNone) {
    /* try a device with a different number of channels */

    for (i = 0; i < AuServerNumDevices(server); ++i) {
      if (AuDeviceKind(AuServerDevice(server, i)) ==
	  AuComponentKindPhysicalOutput &&
	  AuDeviceNumTracks(AuServerDevice(server, i)) >= 1 &&
	  AuDeviceNumTracks(AuServerDevice(server, i)) <= 2) {
	device    = AuDeviceIdentifier(AuServerDevice(server, i));
	*channels = AuDeviceNumTracks(AuServerDevice(server, i));
	break;
      }
    }
  }

  return device;
}

static
void send(unsigned int len)
{
  if (len > buffer.len)
    len = buffer.len;

  AuWriteElement(server, flow, 0, len, buffer.data, AuFalse, 0);
# if defined(DEBUG)
  fprintf(stderr, "AuWriteElement(..., %u, ...)\n", len);
# endif

  buffer.data += len;
  buffer.len  -= len;
}

static
AuBool eventhandler(AuServer *server, AuEvent *event,
		    AuEventHandlerRec *handler)
{
  AuElementNotifyEvent *notifyevent;

  switch (event->type) {
  case AuEventTypeElementNotify:
# if defined(DEBUG)
    fprintf(stderr, "AuEventTypeElementNotify: ");
# endif
    notifyevent = (AuElementNotifyEvent *) event;

    switch (notifyevent->kind) {
    case AuElementNotifyKindLowWater:
# if defined(DEBUG)
      fprintf(stderr, "AuElementNotifyKindLowWater\n");
# endif
      send(notifyevent->num_bytes);
      break;

    case AuElementNotifyKindState:
# if defined(DEBUG)
      fprintf(stderr, "AuElementNotifyKindState: ");
# endif
      switch (notifyevent->cur_state) {
      case AuStatePause:
# if defined(DEBUG)
	fprintf(stderr, "AuStatePause\n");
# endif
	if (notifyevent->reason != AuReasonUser)
	  send(notifyevent->num_bytes);
	break;

      default:
# if defined(DEBUG)
	fprintf(stderr, "other\n");
# endif
      }
      break;

    default:
# if defined(DEBUG)
      fprintf(stderr, "other\n");
# endif
    }
    break;

  default:
# if defined(DEBUG)
    fprintf(stderr, "other\n");
# endif
  }

  return AuTrue;
}

static
int config(struct audio_config *config)
{
  unsigned int bitdepth;
  int format = 0;
  AuDeviceID device;
  AuElement elements[2];

  bitdepth = config->precision & ~7;
  if (bitdepth == 0 || bitdepth > 16)
    bitdepth = 16;

  switch (config->precision = bitdepth) {
  case 8:
    format    = AuFormatLinearSigned8;
    audio_pcm = audio_pcm_s8;
    break;

  case 16:
# if defined(WORDS_BIGENDIAN)
    format    = AuFormatLinearSigned16MSB;
    audio_pcm = audio_pcm_s16be;
# else
    format    = AuFormatLinearSigned16LSB;
    audio_pcm = audio_pcm_s16le;
# endif
    break;
  }

  device = getdevice(server, &config->channels);
  if (device == AuNone) {
    audio_error = _("could not find an output device");
    return -1;
  }

  AuMakeElementImportClient(&elements[0], config->speed, format,
			    config->channels, AuTrue,
			    config->speed * 2, config->speed, 0, 0);
  AuMakeElementExportDevice(&elements[1], 0, device, config->speed,
			    AuUnlimitedSamples, 0, 0);
  AuSetElements(server, flow, AuTrue, 2, elements, 0);

  AuRegisterEventHandler(server, AuEventHandlerIDMask, 0, flow,
			 eventhandler, 0);

  AuStartFlow(server, flow, 0);

  return 0;
}

static
int play(struct audio_play *play)
{
  unsigned char data[MAX_NSAMPLES * 4 * 2];
  unsigned int len;

  len = audio_pcm(data, play->nsamples, play->samples[0], play->samples[1],
		  play->mode, play->stats);

  buffer.data = data;
  buffer.len  = len;

  while (buffer.len)
    AuHandleEvents(server);

  return 0;
}

static
int stop(struct audio_stop *stop)
{
  return 0;
}

static
int finish(struct audio_finish *finish)
{
  AuCloseServer(server);

  return 0;
}

int audio_nas(union audio_control *control)
{
  audio_error = 0;

  switch (control->command) {
  case AUDIO_COMMAND_INIT:
    return init(&control->init);

  case AUDIO_COMMAND_CONFIG:
    return config(&control->config);

  case AUDIO_COMMAND_PLAY:
    return play(&control->play);

  case AUDIO_COMMAND_STOP:
    return stop(&control->stop);

  case AUDIO_COMMAND_FINISH:
    return finish(&control->finish);
  }

  return 0;
}
