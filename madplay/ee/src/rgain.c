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

# ifdef HAVE_CONFIG_H
#  include "config.h"
# endif

# include "global.h"

# include <mad.h>

# include "rgain.h"
# include "gettext.h"

/*
 * NAME:	rgain->parse()
 * DESCRIPTION:	parse a 16-bit Replay Gain field
 */
void rgain_parse(struct rgain *rgain, struct mad_bitptr *ptr)
{
  int negative;

  rgain->name       = mad_bit_read(ptr, 3);
  rgain->originator = mad_bit_read(ptr, 3);

  negative          = mad_bit_read(ptr, 1);
  rgain->adjustment = mad_bit_read(ptr, 9);

  if (negative)
    rgain->adjustment = -rgain->adjustment;
}

/*
 * NAME:	rgain->originator()
 * DESCRIPTION:	return a string description of a Replay Gain originator
 */
char const *rgain_originator(struct rgain const *rgain)
{
  char const *originator = 0;

  switch (rgain->originator) {
  case RGAIN_ORIGINATOR_UNSPECIFIED:
    return 0;
  case RGAIN_ORIGINATOR_PRESET:
    originator = _("preset");
    break;
  case RGAIN_ORIGINATOR_USER:
    originator = _("user");
    break;
  case RGAIN_ORIGINATOR_AUTOMATIC:
    originator = _("automatic");
    break;
  }

  return originator ? originator : _("other");
}
