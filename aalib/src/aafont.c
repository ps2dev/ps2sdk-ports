#include <stdio.h>
#include <ctype.h>
#include "config.h"
#include "aalib.h"
#include "aaint.h"

static double DIMC,CONSTANT;
#define isset(n,i) (((i)&1<<(n))!=0)
#define canset(n,i) (!isset(n,i)&&isset(n-1,i))
#define MUL 8

static struct aa_font *currfont;
static void values(int c, int *v1, int *v2, int *v3, int *v4)
{
    int i;
    int attr = c / 256;
    unsigned const char *font;
    font = currfont->data;
    font = currfont->data;
    c = c % 256;
    c = c * currfont->height;
    *v1 = 0;
    *v2 = 0;
    *v3 = 0;
    *v4 = 0;
    for (i = 0; i < currfont->height / 2; i++) {
	*v1 += (isset(0, font[c + i]) +
		isset(1, font[c + i]) +
		isset(2, font[c + i]) +
		isset(3, font[c + i]));
	*v2 += (isset(4, font[c + i]) +
		isset(5, font[c + i]) +
		isset(6, font[c + i]) +
		isset(7, font[c + i]));
    }
    for (; i < currfont->height; i++) {
	*v3 += (isset(0, font[c + i]) +
		isset(1, font[c + i]) +
		isset(2, font[c + i]) +
		isset(3, font[c + i]));
	*v4 += (isset(4, font[c + i]) +
		isset(5, font[c + i]) +
		isset(6, font[c + i]) +
		isset(7, font[c + i]));
    }
    (*v1) *= MUL;
    (*v2) *= MUL;
    (*v3) *= MUL;
    (*v4) *= MUL;
    switch (attr) {
    case AA_REVERSE:
	*v1 = currfont->height * 2 * MUL - *v1;
	*v2 = currfont->height * 2 * MUL - *v2;
	*v3 = currfont->height * 2 * MUL - *v3;
	*v4 = currfont->height * 2 * MUL - *v4;
	break;
    case AA_DIM:
	*v1 = (*v1 + 1) / DIMC;
	*v2 = (*v2 + 1) / DIMC;
	*v3 = (*v3 + 1) / DIMC;
	*v4 = (*v4 + 1) / DIMC;
	break;
    case AA_BOLD:
	*v1 = *v1 * CONSTANT;
	*v2 = *v2 * CONSTANT;
	*v3 = *v3 * CONSTANT;
	*v4 = *v4 * CONSTANT;
	break;
    case AA_BOLDFONT:
	for (i = 0; i < currfont->height / 2; i++) {
	    *v1 += ((isset(0, font[c + i]) +
		     canset(1, font[c + i]) +
		     canset(2, font[c + i]) +
		     canset(3, font[c + i]))) * MUL;
	    *v2 += ((isset(4, font[c + i]) +
		     canset(5, font[c + i]) +
		     canset(6, font[c + i]) +
		     canset(7, font[c + i]))) * MUL;
	}
	for (; i < currfont->height; i++) {
	    *v3 += ((isset(0, font[c + i]) +
		     canset(1, font[c + i]) +
		     canset(2, font[c + i]) +
		     canset(3, font[c + i]))) * MUL;
	    *v4 += ((isset(4, font[c + i]) +
		     canset(5, font[c + i]) +
		     canset(6, font[c + i]) +
		     canset(7, font[c + i]))) * MUL;
	}
    }
}
void aa_calcparams(struct aa_font *font, struct parameters *parameters, int supported,double dimmul, double boldmul)
{
    int i;
    int ma1 = 0, ma2 = 0, ma3 = 0, ma4 = 0, msum = 0;
    int mi1 = 50000, mi2 = 50000, mi3 = 50000, mi4 = 50000, misum = 50000;
    int v1, v2, v3, v4, sum;
    DIMC=dimmul;
    CONSTANT=boldmul;
    currfont = font;
    for (i = 0; i < NCHARS; i++) {
	if (!ALOWED(i, supported))
	    continue;
	values(i, &v1, &v2, &v3, &v4);
	if (v1 > ma1)
	    ma1 = v1;
	if (v2 > ma2)
	    ma2 = v2;
	if (v3 > ma3)
	    ma3 = v3;
	if (v4 > ma4)
	    ma4 = v4;
	if (v1 + v2 + v3 + v4 > msum)
	    msum = v1 + v2 + v3 + v4;
	if (v1 < mi1)
	    mi1 = v1;
	if (v2 < mi2)
	    mi2 = v2;
	if (v3 < mi3)
	    mi3 = v3;
	if (v4 < mi4)
	    mi4 = v4;
	if (v1 + v2 + v3 + v4 < misum)
	    misum = v1 + v2 + v3 + v4;
    }
    msum -= misum;
#if 0
    ma1 -= mi1;
    ma2 -= mi2;
    ma3 -= mi3;
    ma4 -= mi4;
#else
    mi1 = misum / 4;
    mi2 = misum / 4;
    mi3 = misum / 4;
    mi4 = misum / 4;
    ma1 = msum / 4;
    ma2 = msum / 4;
    ma3 = msum / 4;
    ma4 = msum / 4;
#endif
    for (i = 0; i < NCHARS; i++) {
	if (!ALOWED1(i, supported))
	    continue;
	values(i, &v1, &v2, &v3, &v4);
	sum = ((double) (v1 + v2 + v3 + v4 - misum) * (1020 / (double) msum) + 0.5);
	v1 = ((double) (v1 - mi1) * (255 / (double) ma1) + 0.5);
	v2 = ((double) (v2 - mi2) * (255 / (double) ma2) + 0.5);
	v3 = ((double) (v3 - mi3) * (255 / (double) ma3) + 0.5);
	v4 = ((double) (v4 - mi4) * (255 / (double) ma4) + 0.5);
#if 0
	sum = v1 + v2 + v3 + v4;
#endif
	if (v1 > 255)
	    v1 = 255;
	if (v2 > 255)
	    v2 = 255;
	if (v3 > 255)
	    v3 = 255;
	if (v4 > 255)
	    v4 = 255;
	if (v1 < 0)
	    v1 = 0;
	if (v2 < 0)
	    v2 = 0;
	if (v3 < 0)
	    v3 = 0;
	if (v4 < 0)
	    v4 = 0;
	parameters[i].p[0] = v1;
	parameters[i].p[1] = v2;
	parameters[i].p[2] = v3;
	parameters[i].p[3] = v4;
	parameters[i].p[4] = sum;
    }
}
