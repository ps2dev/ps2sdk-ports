#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "aalib.h"
#include "aamktabl.h"
#include "config.h"
aa_renderparams aa_defrenderparams =
{0, 0, 1.0, AA_FLOYD_S, 0, 0};
#define VAL (13)		/*maximum distance good for fill tables */
#define pos(i1,i2,i3,i4) (((int)(i1)<<12)+((int)(i2)<<8)+((int)(i3)<<4)+(int)(i4))
char *aa_dithernames[] =
{
    "no dithering",
    "error-distribution",
    "floyd-steelberg dithering",
    NULL
};

#define DO_CONTRAST(i,c) (i<c?0:(i>256-c)?255:(i-c)*255/(255-2*c))
aa_renderparams *aa_getrenderparams(void)
{
    aa_renderparams *p = calloc(1, sizeof(*p));
    if (p == NULL)
	return NULL;
    *p = aa_defrenderparams;
    return (p);
}
#define MYLONG_MAX 0xffffffff     /*this is enought for me. */
#define myrand() (state = ((state * 1103515245) + 12345) & MYLONG_MAX)

void aa_renderpalette(aa_context * c, aa_palette palette, aa_renderparams * p, int x1, int y1, int x2, int y2)
{
    static int state;
    int x, y;
    int val;
    int wi = c->imgwidth;
    int pos;
    int i;
    int pos1;
    int i1, i2, i3, i4, esum;
    int *errors[2];
    int cur = 0;
    int mval;
    int gamma = p->gamma != 1.0;
    aa_palette table;
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
    if (p->dither == AA_FLOYD_S) {
	errors[0] = calloc(1, (x2 + 5) * sizeof(int));
	if (errors[0] == NULL)
	    p->dither = AA_ERRORDISTRIB;
	errors[0] += 3;
	errors[1] = calloc(1, (x2 + 5) * sizeof(int));
	if (errors[1] == NULL)
	    free(errors[0]), p->dither = AA_ERRORDISTRIB;
	errors[1] += 3;
	cur = 0;
    }
    for (i = 0; i < 256; i++) {
	y = palette[i] + p->bright;
	if (y > 255)
	    y = 255;
	if (y < 0)
	    y = 0;
	if (p->contrast)
	    y = DO_CONTRAST(y, p->contrast);
	if (gamma) {
	    y = pow(y / 255.0, p->gamma) * 255 + 0.5;
	}
	if (p->inversion)
	    y = 255 - y;
	if (y > 255)
	    y = 255;
	else if (y < 0)
	    y = 0;
	table[i] = y;
    }
    gamma = 0;
    if (p->randomval)
	gamma = p->randomval / 2;
    mval = (c->parameters[c->filltable[255]].p[4]);
    for (y = y1; y < y2; y++) {
	pos = 2 * y * wi;
	pos1 = y * aa_scrwidth(c);
	esum = 0;
	for (x = x1; x < x2; x++) {
	    i1 = table[((((int) c->imagebuffer[pos])))];
	    i2 = table[((((int) c->imagebuffer[pos + 1])))];
	    i3 = table[((((int) c->imagebuffer[pos + wi])))];
	    i4 = table[((((int) c->imagebuffer[pos + 1 + wi])))];
	    if (gamma) {
		i = myrand();
		i1 += (i) % p->randomval - gamma;
		i2 += (i >> 8) % p->randomval - gamma;
		i3 += (i >> 16) % p->randomval - gamma;
		i4 += (i >> 24) % p->randomval - gamma;
		if ((i1 | i2 | i3 | i4) & (~255)) {
		    if (i1 < 0)
			i1 = 0;
		    else if (i1 > 255)
			i1 = 255;
		    if (i2 < 0)
			i2 = 0;
		    else if (i2 > 255)
			i2 = 255;
		    if (i3 < 0)
			i3 = 0;
		    else if (i3 > 255)
			i3 = 255;
		    if (i4 < 0)
			i4 = 0;
		    else if (i4 > 255)
			i4 = 255;
		}
	    }
	    switch (p->dither) {
	    case AA_ERRORDISTRIB:
		esum = (esum + 2) >> 2;
		i1 += esum;
		i2 += esum;
		i3 += esum;
		i4 += esum;
		break;
	    case AA_FLOYD_S:
		if (i1 | i2 | i3 | i4) {
		    errors[cur][x - 2] += esum >> 4;
		    errors[cur][x - 1] += (5 * esum) >> 4;
		    errors[cur][x] = (3 * esum) >> 4;
		    esum = (7 * esum) >> 4;
		    esum += errors[cur ^ 1][x];
		    i1 += (esum + 1) >> 2;
		    i2 += (esum) >> 2;
		    i3 += (esum + 3) >> 2;
		    i4 += (esum + 2) >> 2;
		}
		break;
	    }
	    if (p->dither) {
		esum = i1 + i2 + i3 + i4;
		val = (esum) >> 2;
		if ((abs(i1 - val) < VAL &&
		     abs(i2 - val) < VAL &&
		     abs(i3 - val) < VAL &&
		     abs(i4 - val) < VAL)) {
		    if (esum >= 4 * 256)
			val = 255, esum = 4 * 256 - 1;
		    if (val < 0)
			val = 0;
		    val = c->filltable[val];
		} else {
		    if ((i1 | i2 | i3 | i4) & (~255)) {
			if (i1 < 0)
			    i1 = 0;
			else if (i1 > 255)
			    i1 = 255;
			if (i2 < 0)
			    i2 = 0;
			else if (i2 > 255)
			    i2 = 255;
			if (i3 < 0)
			    i3 = 0;
			else if (i3 > 255)
			    i3 = 255;
			if (i4 < 0)
			    i4 = 0;
			else if (i4 > 255)
			    i4 = 255;
		    }
		    esum = i1 + i2 + i3 + i4;
		    i1 >>= 4;
		    i2 >>= 4;
		    i3 >>= 4;
		    i4 >>= 4;
		    val = c->table[pos(i2, i1, i4, i3)];
		}
		esum = (esum - (c->parameters[val].p[4]) * 1020 / mval);
	    } else {
		val = (i1 + i2 + i3 + i4) >> 2;
		if ((abs(i1 - val) < VAL &&
		     abs(i2 - val) < VAL &&
		     abs(i3 - val) < VAL &&
		     abs(i4 - val) < VAL)) {
		    val = c->filltable[val];
		} else {
		    i1 >>= 4;
		    i2 >>= 4;
		    i3 >>= 4;
		    i4 >>= 4;
		    val = c->table[pos(i2, i1, i4, i3)];
		}
	    }
	    c->attrbuffer[pos1] = val >> 8;
	    c->textbuffer[pos1] = val & 0xff;
	    pos += 2;
	    pos1++;
	}
	if (p->dither == AA_FLOYD_S) {
	    if (x2 - 1 > x1)
		errors[cur][x2 - 2] += (esum) >> 4;
	    if (x2 > x1)
		errors[cur][x2 - 1] += (5 * esum) >> 4;
	    cur ^= 1;
	    errors[cur][x1] = 0;
	    errors[cur ^ 1][-1] = 0;
	}
    }
    if (p->dither == AA_FLOYD_S) {
	free(errors[0] - 3);
	free(errors[1] - 3);
    }
}
void aa_render(aa_context * c, aa_renderparams * p, int x1, int y1, int x2, int y2)
{
    int i;
    static aa_palette table;
    if (table[255] != 255)
	for (i = 0; i < 256; i++) {
	    table[i] = i;
	}
    aa_renderpalette(c, table, p, x1, y1, x2, y2);
}
