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

# include <ctype.h>

/*
 * NAME:	strncasecmp()
 * DESCRIPTION:	compare a portion of two strings without regard to case
 */
int strncasecmp(char const *str1, char const *str2, unsigned long len)
{
  signed int c1 = 0, c2 = 0;

  while (len--) {
    c1 = tolower(*str1++);
    c2 = tolower(*str2++);

    if (c1 == 0 || c1 != c2)
      break;
  }

  return c1 - c2;
}
