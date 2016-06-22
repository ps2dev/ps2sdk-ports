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
 * $Id: filter.h,v 1.10 2004/02/17 02:26:43 rob Exp $
 */

# ifndef FILTER_H
# define FILTER_H

# include <mad.h>

typedef enum mad_flow filter_func_t(void *, struct mad_frame *);

struct filter {
  int flags;
  filter_func_t *func;
  void *data;
  struct filter *chain;
};

enum {
  FILTER_FLAG_DMEM = 0x0001
};

void filter_init(struct filter *, filter_func_t *, void *, struct filter *);

# define filter_finish(filter)	/* nothing */

struct filter *filter_new(filter_func_t *, void *, struct filter *);
void filter_free(struct filter *);

enum mad_flow filter_run(struct filter *, struct mad_frame *);

/* filter function prototypes */

filter_func_t gain_filter;		/* mad_fixed_t *data */
filter_func_t limit_filter;		/* struct player *data */
filter_func_t mono_filter;		/* void *data */
filter_func_t fadein_filter;		/* struct player *data */

# if defined(EXPERIMENTAL)
filter_func_t mixer_filter;		/* FILE *data */
filter_func_t experimental_filter;	/* void *data */
# endif

# endif
