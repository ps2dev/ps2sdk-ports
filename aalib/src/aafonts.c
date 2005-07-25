#include <stdio.h>
#include "config.h"
#include "aalib.h"
#define MAXFONTS 246
struct aa_font *aa_fonts[MAXFONTS + 1] =
{
#ifdef VYHEN_SUPPORT
    &aa_fontvyhen,
#endif
    &aa_font8, &aa_font9, &aa_font14, &aa_font16, &aa_fontX13, &aa_fontX13B, &aa_fontX16, &aa_fontline, &aa_fontgl, &aa_fontcourier, 
    NULL
};

int aa_registerfont(struct aa_font *f)
{
    int i;
    for (i = 0; i < 256 && aa_fonts[i] != NULL; i++);
    if (i == 256)
	return 0;
    aa_fonts[i] = f;
    aa_fonts[i + 1] = 0;
    return (1);
}
