#include <setjmp.h>
#include <signal.h>
#include <stdio.h>
#include "config.h"
#ifdef SLANG_DRIVER
#include <slang.h>
#ifdef GPM_MOUSEDRIVER
#include <gpm.h>
#endif
#include "aalib.h"
struct aa_driver slang_d;
int __slang_is_up = 0;
int __resized_slang = 0;
static int uninitslang;

static int slang_init(struct aa_hardware_params *p, void *none, struct aa_hardware_params *dest, void **params)
{
    struct aa_hardware_params def={NULL, AA_NORMAL_MASK | AA_BOLD_MASK | AA_REVERSE_MASK | AA_BOLDFONT_MASK | AA_DIM_MASK};
    *dest=def;
    fflush(stdout);
    if (!__slang_is_up) {
	SLtt_get_terminfo();
	__slang_is_up = 1;
	uninitslang = 1;
    }
    if (!SLsmg_init_smg())
	return 0;
    if (SLtt_Use_Ansi_Colors) {
	dest->supported &= ~AA_BOLDFONT_MASK;
    }
    SLsmg_Display_Eight_Bit = 128;
    dest->supported |= AA_EIGHT;
    aa_recommendlowkbd("slang");
    return 1;
}
static void slang_uninit(aa_context * c)
{
    SLsmg_reset_smg();
    if (uninitslang) {
	uninitslang = 0;
	__slang_is_up = 0;
    }
}
static void slang_getsize(aa_context * c, int *width, int *height)
{
    SLtt_get_screen_size();
    SLsmg_reset_smg();
    if (!SLsmg_init_smg())
	printf("Internal error!\n");
    SLtt_set_mono(AA_NORMAL, "normal", 0);
    SLtt_set_mono(AA_BOLD, "bold", SLTT_BOLD_MASK);
    SLtt_set_mono(AA_DIM, "dim", SLTT_ALTC_MASK);
    SLtt_set_mono(AA_REVERSE, "reverse", SLTT_REV_MASK);
    SLtt_set_mono(AA_SPECIAL, "special", 0);
    SLtt_set_mono(AA_BOLDFONT, "boldfont", SLTT_BOLD_MASK);

    SLtt_set_color(AA_NORMAL, "normal", "lightgray", "black");
    SLtt_set_color(AA_BOLD, "bold", "white", "black");
    SLtt_set_color(AA_DIM, "dim", "gray", "black");
    SLtt_set_color(AA_REVERSE, "bold", "black", "lightgray");
    SLtt_set_color(AA_SPECIAL, "dim", "lightgray", "blue");
    SLtt_set_color(AA_BOLDFONT, "bold", "white", "black");
    *width = SLtt_Screen_Cols;
    *height = SLtt_Screen_Rows;
    /*if(i==2) exit(1); */
#ifdef GPM_MOUSEDRIVER
    gpm_mx = *width;
    gpm_my = *height;
#endif

}
static void slang_setattr(aa_context * c, int attr)
{
    SLsmg_set_color(attr);
}
static void slang_print(aa_context * c, char *text)
{
    SLsmg_write_string(text);
}
static void slang_flush(aa_context * c)
{
    SLsmg_refresh();
}
static void slang_gotoxy(aa_context * c, int x, int y)
{
    SLsmg_gotorc(y, x);
}
static void slang_cursor(aa_context * c, int mode)
{
    SLtt_set_cursor_visibility(mode);
}



struct aa_driver slang_d =
{
    "slang", "Slang driver 1.0",
    slang_init,
    slang_uninit,
    slang_getsize,
    slang_setattr,
    slang_print,
    slang_gotoxy,
    slang_flush,
    slang_cursor,
};
#endif
