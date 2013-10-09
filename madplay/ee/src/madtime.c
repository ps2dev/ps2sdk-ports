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
 * $Id: madtime.c,v 1.25 2004/01/23 09:41:32 rob Exp $
 */

# ifdef HAVE_CONFIG_H
#  include "config.h"
# endif

# include "global.h"

# if !defined(HAVE_MMAP)
#  error "madtime currently requires mmap() support"
# endif

# include <stdio.h>
# include <stdlib.h>

# ifdef HAVE_SYS_TYPES_H
#  include <sys/types.h>
# endif

# include <sys/stat.h>

# ifdef HAVE_FCNTL_H
#  include <fcntl.h>
# endif

# include <unistd.h>
# include <string.h>
# include <sys/mman.h>
# include <locale.h>
# include <mad.h>

# include "getopt.h"
# include "gettext.h"

# if !defined(O_BINARY)
#  define O_BINARY  0
# endif

static
signed int scan(unsigned char const *ptr, unsigned long len,
		mad_timer_t *duration)
{
  struct mad_stream stream;
  struct mad_header header;
  unsigned long bitrate, kbps, count;
  int vbr;

  mad_stream_init(&stream);
  mad_header_init(&header);

  mad_stream_buffer(&stream, ptr, len);

  bitrate = kbps = count = vbr = 0;

  while (1) {
    if (mad_header_decode(&header, &stream) == -1) {
      if (MAD_RECOVERABLE(stream.error))
	continue;
      else
	break;
    }

    if (bitrate && header.bitrate != bitrate)
      vbr = 1;

    bitrate = header.bitrate;

    kbps += bitrate / 1000;
    ++count;

    mad_timer_add(duration, header.duration);
  }

  mad_header_finish(&header);
  mad_stream_finish(&stream);

  if (count == 0)
    count = 1;

  return ((kbps * 2) / count + 1) / 2 * (vbr ? -1 : 1);
}

static
int calc(char const *path, mad_timer_t *duration,
	 signed int *kbps, unsigned long *kbytes)
{
  int fd;
  struct stat stat;
  void *fdm;

  fd = open(path, O_RDONLY | O_BINARY);
  if (fd == -1) {
    perror(path);
    return -1;
  }

  if (fstat(fd, &stat) == -1) {
    perror("fstat");
    close(fd);
    return -1;
  }

  if (!S_ISREG(stat.st_mode)) {
    fprintf(stderr, _("%s: Not a regular file\n"), path);
    close(fd);
    return -1;
  }

  *kbytes = (stat.st_size + 512) / 1024;

  fdm = mmap(0, stat.st_size, PROT_READ, MAP_SHARED, fd, 0);
  if (fdm == MAP_FAILED) {
    perror("mmap");
    close(fd);
    return -1;
  }

  if (fdm) {
    *kbps = scan(fdm, stat.st_size, duration);

    if (munmap(fdm, stat.st_size) == -1) {
      perror("munmap");
      close(fd);
      return -1;
    }
  }
  else
    *kbps = 0;

  if (close(fd) == -1) {
    perror("close");
    return -1;
  }

  return 0;
}

static
void show(mad_timer_t duration, signed int kbps,
	  unsigned long kbytes, char const *label)
{
  char duration_str[19];

  mad_timer_string(duration, duration_str,
		   "%4lu:%02u:%02u.%1u", MAD_UNITS_HOURS,
		   MAD_UNITS_DECISECONDS, 0);

# if defined(HAVE_LOCALECONV)
  {
    char *point;

    point = strchr(duration_str, '.');
    if (point)
      *point = *localeconv()->decimal_point;
  }
# endif

  printf(_("%8.1f MB  %c%3u kbps  %s  %s\n"), kbytes / 1024.0,
	 kbps < 0 ? '~' : ' ', abs(kbps), duration_str, label);
}

static
void usage(char const *argv0)
{
  fprintf(stderr, _("Usage: %s [-s] FILE [...]\n"), argv0);
}

/*
 * NAME:	main()
 * DESCRIPTION:	program entry point
 */
int main(int argc, char *argv[])
{
  mad_timer_t total;
  unsigned long total_kbps, total_kbytes, count;
  signed int bitrate;
  int vbr, opt, i, sum_only = 0;

  /* internationalization support */

# if defined(ENABLE_NLS)
  setlocale(LC_ALL, "");
  bindtextdomain(PACKAGE, LOCALEDIR);
  textdomain(PACKAGE);
# endif

  /* initialize and get options */

  if (argc > 1) {
    if (strcmp(argv[1], "--version") == 0) {
      printf("%s - %s\n", mad_version, mad_copyright);
      printf(_("Build options: %s\n"), mad_build);
      return 0;
    }
    if (strcmp(argv[1], "--help") == 0) {
      usage(argv[0]);
      return 0;
    }
  }

  while ((opt = getopt(argc, argv, "s")) != -1) {
    switch (opt) {
    case 's':
      sum_only = 1;
      break;

    default:
      usage(argv[0]);
      return 1;
    }
  }

  if (optind == argc) {
    usage(argv[0]);
    return 1;
  }

  /* main processing */

  total = mad_timer_zero;

  total_kbps = total_kbytes = count = bitrate = vbr = 0;

  for (i = optind; i < argc; ++i) {
    mad_timer_t duration = mad_timer_zero;
    signed int kbps;
    unsigned long kbytes;

    if (calc(argv[i], &duration, &kbps, &kbytes) == -1)
      continue;

    if (!sum_only)
      show(duration, kbps, kbytes, argv[i]);

    mad_timer_add(&total, duration);

    total_kbytes += kbytes;

    if (kbps) {
      total_kbps += abs(kbps);
      ++count;
    }

    if (kbps < 0 || (bitrate && kbps != bitrate))
      vbr = 1;

    bitrate = kbps;
  }

  if (count == 0)
    count = 1;

  if (argc > 2 || sum_only) {
    show(total, ((total_kbps * 2) / count + 1) / 2 * (vbr ? -1 : 1),
	 total_kbytes, _("TOTAL"));
  }

  return 0;
}
