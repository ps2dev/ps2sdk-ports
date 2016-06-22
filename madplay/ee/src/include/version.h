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
 * $Id: version.h,v 1.31 2004/02/23 21:34:53 rob Exp $
 */

# ifndef VERSION_H
# define VERSION_H

# include <stdio.h>

# define MADPLAY_VERSION	"0.15.2 (beta)"

# define MADPLAY_PUBLISHYEAR	"2000-2004"
# define MADPLAY_AUTHOR		"Robert Leslie"
# define MADPLAY_EMAIL		"rob@mars.org"

extern char const madplay_version[];
extern char const madplay_copyright[];
extern char const madplay_author[];
extern char const madplay_build[];

void ver_banner(FILE *);
void ver_version(FILE *);
void ver_license(FILE *);

# endif
