#include "config.h"
#include <stdio.h>
#include "aalib.h"
#include "debug.h"

static int stdout_init(struct aa_hardware_params *p, void *none, struct aa_hardware_params *dest, void **n)
{
    static struct aa_hardware_params def={NULL, AA_NORMAL_MASK | AA_EXTENDED};
    *dest=def;
    init_scr();
    scr_clear();
    return 1;
}
static void stdout_uninit(aa_context * c)
{
}
static void stdout_getsize(aa_context * c, int *width, int *height)
{
}

static void stdout_flush(aa_context * c)
{
    int x, y;
    for (y = 0; y < aa_scrheight(c); y++) {
	for (x = 0; x < aa_scrwidth(c)-2; x++) {
	    scr_printf("%c",c->textbuffer[x + y * aa_scrwidth(c)]);
	}
	scr_printf(" \n");
    }
    scr_printf(" \n");
    scr_clear();
}
static void stdout_gotoxy(aa_context * c, int x, int y)
{
}
struct aa_driver stdout_d =
{
    "stdout", "Standard output driver",
    stdout_init,
    stdout_uninit,
    stdout_getsize,
    NULL,
    NULL,
    stdout_gotoxy,
    stdout_flush,
    NULL
};


static void stderr_flush(aa_context * c)
{
    int x, y;
    for (y = 0; y < aa_scrheight(c); y++) {
	for (x = 0; x < aa_scrwidth(c); x++) {
	    putc(c->textbuffer[x + y * aa_scrwidth(c)], stderr);
	}
	putc('\n', stderr);
    }
    putc('', stderr);
    putc('\n', stderr);
    fflush(stderr);
}
struct aa_driver stderr_d =
{
    "stderr", "Standard error driver",
    stdout_init,
    stdout_uninit,
    stdout_getsize,
    NULL,
    NULL,
    stdout_gotoxy,
    stderr_flush,
    NULL
};
