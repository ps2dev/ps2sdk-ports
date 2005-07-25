#ifndef __AAINT_INCLUDED__
#define __AAINT_INCLUDED__
#include <stdio.h>

#define NCHARS (256*AA_NATTRS)
#define aa_validmode(x,y,params)  \
      ((((params)->minwidth||(params)->maxwidth)||((params)->width==(x)||!(params)->width))&& \
      (((params)->minheight||(params)->maxheight)||((params)->height==(y)||!(params)->height)) && \
      ((params)->minwidth?(params->minwidth)<=(x):1)&& \
      ((params)->minheight?(params->minheight)<=(x):1)&& \
      ((params)->maxwidth?(params->maxwidth)>=(x):1)&& \
      ((params)->maxheight?(params->maxheight)>=(x):1))

#define ALOWED(i,s) ((isgraph((i)&0xff)||(((i)&0xff)==' ')||(((i)&0xff)>160&&(s&AA_EIGHT))||((s&AA_ALL)&&((i)&0xff)))&&(s&TOMASK(((i)>>8))))
#define ALOWED1(i,s) (1)
#define TOMASK(i) (1<<(i))
#endif
