#ifndef __AAMKTABLE_INCLUDED__
#define __AAMKTABLE_INCLUDED__
#include "aalib.h"
unsigned short *aa_mktable(aa_context * c);
void aa_calcparams(struct aa_font *font, struct parameters *parameters, int supported, double dimmul, double boldmul);
#endif
