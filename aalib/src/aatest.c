#include "aalib.h"
int main(int argc, char **argv)
{
    aa_context *c;
    int i, y;
    char s[256];
    aa_renderparams *p;
    strcpy(s, "line editor.");
    if (!aa_parseoptions(NULL, NULL, &argc, argv) || argc != 1) {
	printf("%s\n", aa_help);
	exit(1);
    }
    c = aa_autoinit(&aa_defparams);
    if (c == NULL) {
	printf("Can not intialize aalib\n");
	exit(2);
    }
    if (!aa_autoinitkbd(c, 0)) {
	printf("Can not intialize keyboard\n");
	aa_close(c);
	exit(3);
    }
    for (i = 0; i < aa_imgwidth(c); i++)
	for (y = 0; y < aa_imgheight(c); y++)
	    aa_putpixel(c, i, y, i + y < 80 ? i : ((i + y) < 100 ? (i + y == 89 ? 150 : 0) : y * 8));
    aa_hidecursor(c);
    aa_fastrender(c, 0, 0, aa_scrwidth(c), aa_scrheight(c));
    aa_printf(c, 0, 0, AA_SPECIAL, "Fast rendering routine %i",1);
    aa_flush(c);
    aa_getkey(c, 1);
    aa_edit(c, 0, 1, 20, s, 256);
    aa_puts(c, 0, 0, AA_SPECIAL, "Key lookup test        ");
    aa_flush(c);
    while (aa_getkey(c, 0) == AA_NONE);
    if (aa_autoinitmouse(c, AA_MOUSEALLMASK)) {
	int co = 0;
	sprintf(s, "Mouse test-space to exit");
	aa_puts(c, 0, 0, AA_SPECIAL, s);
	aa_flush(c);
	while (aa_getevent(c, 1) != ' ') {
	    int x, y, b;
	    char s[80];
	    co++;
	    aa_getmouse(c, &x, &y, &b);
	    sprintf(s, "Mouse test-space to exit. x:%i y:%i b:%i event #%i  ", x, y, b, co);
	    aa_puts(c, 0, 0, AA_SPECIAL, s);
	    aa_flush(c);
	}
        aa_hidemouse(c);
	while (aa_getevent(c, 1) != ' ') {
	    int x, y, b;
	    char s[80];
	    co++;
	    aa_getmouse(c, &x, &y, &b);
	    sprintf(s, "Hidden mouse test-space to exit. x:%i y:%i b:%i event #%i  ", x, y, b, co);
	    aa_puts(c, 0, 0, AA_SPECIAL, s);
	    aa_flush(c);
	}
	aa_uninitmouse(c);
    }
    p = aa_getrenderparams();
    for (i = 0; i < AA_DITHERTYPES; i++) {
	p->dither = i;
	aa_render(c, p, 0, 0, aa_scrwidth(c), aa_scrheight(c));
	aa_puts(c, 0, 0, AA_SPECIAL, aa_dithernames[i]);
	aa_flush(c);
	aa_getkey(c, 1);
    }
    for (i = 0; i < 255; i += 32) {
	p->bright = i;
	aa_render(c, p, 0, 0, aa_scrwidth(c), aa_scrheight(c));
	aa_puts(c, 0, 0, AA_SPECIAL, "Normal rendering - bright changes");
	aa_flush(c);
	aa_getkey(c, 1);
    }
    p->bright = 0;
    for (i = 0; i < 128; i += 16) {
	p->contrast = i;
	aa_render(c, p, 0, 0, aa_scrwidth(c), aa_scrheight(c));
	aa_puts(c, 0, 0, AA_SPECIAL, "Normal rendering - contrast changes");
	aa_flush(c);
	aa_getkey(c, 1);
    }
    p->contrast = 0;
    for (i = 0; i < 255; i += 32) {
	p->gamma = 1 + i / 32.0;
	aa_render(c, p, 0, 0, aa_scrwidth(c), aa_scrheight(c));
	aa_puts(c, 0, 0, AA_SPECIAL, "Normal rendering - gamma changes");
	aa_flush(c);
	aa_getkey(c, 1);
    }
    p->gamma = 1.0;
    for (i = 0; i < 255; i += 32) {
	p->randomval = i;
	aa_render(c, p, 0, 0, aa_scrwidth(c), aa_scrheight(c));
	aa_puts(c, 0, 0, AA_SPECIAL, "Normal rendering - randomval changes");
	aa_flush(c);
	aa_getkey(c, 1);
    }
    aa_close(c);
    return 0;
}
