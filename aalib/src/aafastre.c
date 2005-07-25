#include <stdio.h>
#include "aalib.h"
#include "aamktabl.h"
void aa_fastrender(aa_context * c, int x1, int y1, int x2, int y2)
{
    int x, y;
    int val;
    int wi = c->imgwidth;
    int pos;
    int pos1;
    if (x2 < 0 || y2 < 0 || x1 > aa_scrwidth(c) || y1 > aa_scrheight(c))
	return;
    if (x2 >= aa_scrwidth(c))
	x2 = aa_scrwidth(c);
    if (y2 >= aa_scrheight(c))
	y2 = aa_scrheight(c);
    if (x1 < 0)
	x1 = 0;
    if (y1 < 0)
	y1 = 0;
    if (c->table == NULL)
	aa_mktable(c);
    for (y = y1; y < y2; y++) {
	pos = 2 * y * wi;
	pos1 = y * aa_scrwidth(c);
	for (x = x1; x < x2; x++) {
	    val = c->table[
			      ((((int) c->imagebuffer[pos] >> 4)) << 8) +
			 ((((int) c->imagebuffer[pos + 1] >> 4)) << 12) +
			      ((((int) c->imagebuffer[pos + wi] >> 4))) +
		     ((((int) c->imagebuffer[pos + 1 + wi] >> 4)) << 4)];
	    c->attrbuffer[pos1] = val >> 8;
	    c->textbuffer[pos1] = val & 0xff;
	    pos += 2;
	    pos1++;
	}
    }
}
