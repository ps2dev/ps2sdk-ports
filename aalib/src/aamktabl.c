#include <stdio.h>
#include <limits.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "aalib.h"
#include "aaint.h"
#include "aamktabl.h"
#include "config.h"
#define TABLESIZE 65536

#define pow2(i) ((i)*(i))
/*#define pow2(i) (abs(i)) */
#define add(i) if(next[(i)]==(i)&&last!=(i)) {if(last!=-1) next[last]=(i),last=(i); else last=first=(i);}
#define dist(i1,i2,i3,i4,i5,y1,y2,y3,y4,y5) (2*(pow2((int)(i1)-(int)(y1))+pow2((int)(i2)-(int)(y2))+(pow2((int)(i3)-(int)(y3))+pow2((int)(i4)-(int)(y4))))+1*pow2((int)(i5)-(int)(y5)))
#define dist1(i1,i2,i3,i4,i5,y1,y2,y3,y4,y5) ((pow2((int)(i1)-(int)(y1))+pow2((int)(i2)-(int)(y2))+(pow2((int)(i3)-(int)(y3))+pow2((int)(i4)-(int)(y4))))+2*pow2((int)(i5)-(int)(y5)))
#define pos(i1,i2,i3,i4) (((int)(i1)<<12)+((int)(i2)<<8)+((int)(i3)<<4)+(int)(i4))

#define postoparams(pos,i1,i2,i3,i4) \
  ((i1)=(pos)>>12),((i2)=((pos)>>8)&15),((i3)=((pos)>>4)&15),((i4)=((pos))&15)

int priority[]={4,5,3,2,1};

unsigned short *aa_mktable(aa_context * c)
{
    int i;
    int i1, i2, i3, i4;
    int sum, pos;
    struct aa_font *currfont = c->params.font;
    int supported = c->params.supported;
    unsigned short *next;
    int first = -1;
    int last = -1;
    unsigned short *table;
    unsigned short *filltable;
    static struct parameters *parameters;
    next = (unsigned short *) malloc(sizeof(*next) * TABLESIZE);
    parameters = (struct parameters *) calloc(1, sizeof(struct parameters) * (NCHARS + 1));
    table = (unsigned short *) calloc(1, TABLESIZE * sizeof(*table));
    filltable = (unsigned short *) calloc(1, 256 * sizeof(*filltable));
    first = -1;
    last = -1;
    for (i = 0; i < TABLESIZE; i++)
	next[i] = i, table[i] = 0;
    aa_calcparams(currfont, parameters, supported,c->params.dimmul, c->params.boldmul);

    for (i = 0; i < NCHARS; i++) {
	if (ALOWED(i, supported)) {
	    int p1, p2, p3, p4;
	    i1 = parameters[i].p[0];
	    i2 = parameters[i].p[1];
	    i3 = parameters[i].p[2];
	    i4 = parameters[i].p[3];
	    p1 = i1 >> 4;
	    p2 = i2 >> 4;
	    p3 = i3 >> 4;
	    p4 = i4 >> 4;
	    /*
	       if(p1>15) p1=15;
	       if(p2>15) p2=15;
	       if(p3>15) p3=15;
	       if(p4>15) p4=15; */
	    sum = parameters[i].p[4];
	    pos = pos(p1, p2, p3, p4);
	    if (table[pos]) {
		int sum;
		p1 = (p1 << 4) + p1;
		p2 = (p2 << 4) + p2;
		p3 = (p3 << 4) + p3;
		p4 = (p4 << 4) + p4;
		sum = p1 + p2 + p3 + p4;
		if ((p1=dist(parameters[i].p[0],
			 parameters[i].p[1],
			 parameters[i].p[2],
			 parameters[i].p[3],
			 parameters[i].p[4],
			 p1, p2, p3, p4, sum)) >=
		    (p1=dist(parameters[table[pos]].p[0],
			 parameters[table[pos]].p[1],
			 parameters[table[pos]].p[2],
			 parameters[table[pos]].p[3],
			 parameters[table[pos]].p[4],
			 p1, p2, p3, p4, sum))&&(p1!=p2||priority[i/256]<=priority[table[pos]/256]))
		    goto skip;
	    }
	    table[pos] = i;
	    add(pos);
	  skip:;

	}
    }
    for (pos = 0; pos < 256; pos++) {
	int mindist = INT_MAX, d1;
	for (i = 0; i < NCHARS; i++) {
	    if (ALOWED(i, supported)) {
		if ((d1 = dist1(parameters[i].p[0],
				parameters[i].p[1],
				parameters[i].p[2],
				parameters[i].p[3],
				parameters[i].p[4],
				pos, pos, pos, pos, pos*4)) <= mindist&&(d1!=mindist||priority[i/256]>priority[filltable[pos]/256]))
		    filltable[pos] = i, mindist = d1;
	    }
	}
    }
    do {
	int blocked;
	if (last != -1)
	    next[last] = last;
	else
	    break;
	blocked = last;
	i = first;
	if (i == -1)
	    break;
	first = last = -1;
	do {
	    int m1, m2, m3, m4, ii, dm;
	    unsigned short c = table[i];
	    /* printf ("%c\n", c); */
	    postoparams(i, m1, m2, m3, m4);
	    for (dm = 0; dm < 4; dm++)
		for (ii = -1; ii <= 1; ii += 2) {
		    int dist, dist1, index;
		    unsigned short ch;
		    i1 = m1;
		    i2 = m2;
		    i3 = m3;
		    i4 = m4;
		    switch (dm) {
		    case 0:
			i1 += ii;
			if (i1 < 0 || i1 >= 16)
			    continue;
			break;
		    case 1:
			i2 += ii;
			if (i2 < 0 || i2 >= 16)
			    continue;
			break;
		    case 2:
			i3 += ii;
			if (i3 < 0 || i3 >= 16)
			    continue;
			break;
		    case 3:
			i4 += ii;
			if (i4 < 0 || i4 >= 16)
			    continue;
			break;

		    }
		    index = pos(i1, i2, i3, i4);
		    ch = table[index];
		    if (ch == c || index == blocked)
			continue;
		    if (ch) {
			int ii1 = (i1 << 4) + i1;
			int ii2 = (i2 << 4) + i2;
			int ii3 = (i3 << 4) + i3;
			int ii4 = (i4 << 4) + i4;
			int iisum = ii1 + ii2 + ii3 + ii4;
			dist = dist(
				       ii1, ii2, ii3, ii4, iisum,
				       parameters[c].p[0],
				       parameters[c].p[1],
				       parameters[c].p[2],
				       parameters[c].p[3],
				       parameters[c].p[4]);
			dist1 = dist(
					ii1, ii2, ii3, ii4, iisum,
					parameters[ch].p[0],
					parameters[ch].p[1],
					parameters[ch].p[2],
					parameters[ch].p[3],
					parameters[ch].p[4]);
		    }
		    if (!ch || dist < dist1) {
			table[index] = c;
			add(index);
		    }
		}
	    i1 = i;
	    i = next[i];
	    next[i1] = i1;
	}
	while (i != i1);
    }
    while (last != -1);
    c->table = table;
    c->filltable = filltable;
    c->parameters = parameters;
    free(next);
    return (table);
}
