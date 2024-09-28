#include "config.h"
#include "aalib.h"
#include "aaint.h"
#include <debug.h>
#include <stdio.h>

static int stdout_init(__AA_CONST struct aa_hardware_params *p,
                       __AA_CONST void *none, struct aa_hardware_params *dest,
                       void **n) {
  (void)p;
  (void)none;
  (void)n;
  __AA_CONST static struct aa_hardware_params def = {NULL, AA_NORMAL_MASK |
                                                               AA_EXTENDED};
  *dest = def;
  init_scr();
  scr_clear();
  return 1;
}
static void stdout_uninit(aa_context *c) {
  (void)c;
  scr_clear();
}
static void stdout_getsize(aa_context *c, int *width, int *height) {
  (void)c;
  (void)width;
  (void)height;
}

static void stdout_flush(aa_context *c) {
  (void)c;
  int x, y;
  for (y = 0; y < aa_scrheight(c); y++) {
    for (x = 0; x < aa_scrwidth(c) - 2; x++) {
      scr_printf("%c", c->textbuffer[x + y * aa_scrwidth(c)]);
    }
    scr_printf(" \n");
  }
  scr_printf(" \n");
  scr_clear();
}
static void stdout_gotoxy(aa_context *c, int x, int y) { scr_setXY(x, y); }

static void stdout_cursormode(struct aa_context *c, int enable) {
  (void)c;
  scr_setCursor(enable);
}
__AA_CONST struct aa_driver stdout_d = {
    "stdout",       "Standard output driver",
    stdout_init,    stdout_uninit,
    stdout_getsize, NULL,
    NULL,           stdout_gotoxy,
    stdout_flush,   stdout_cursormode};

static void stderr_flush(aa_context *c) {
  (void)c;
  int x, y;
  for (y = 0; y < aa_scrheight(c); y++) {
    for (x = 0; x < aa_scrwidth(c) - 2; x++) {
      scr_printf("%c", c->textbuffer[x + y * aa_scrwidth(c)]);
    }
    scr_printf(" \n");
  }
  scr_printf(" \n");
  scr_clear();
}
__AA_CONST struct aa_driver stderr_d = {"stderr",       "Standard error driver",
                                        stdout_init,    stdout_uninit,
                                        stdout_getsize, NULL,
                                        NULL,           stdout_gotoxy,
                                        stderr_flush,   NULL};
