#include <setjmp.h>
#include <signal.h>
#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include "config.h"
#ifdef SLANG_KBDDRIVER
#include <slang.h>
#ifdef GPM_MOUSEDRIVER
#include <gpm.h>
#endif
#include "aalib.h"
static int iswaiting;
static FILE *f;
static jmp_buf buf;
int __slang_keyboard;
extern int __slang_is_up;
extern int __resized_slang;
/*extern int __curses_usegpm; */
static int uninitslang;
#if 0
int __curses_x, __curses_y, __curses_buttons;
int __curses_keyboard;
#endif

#ifdef SIGWINCH
static void handler(int i)
{
    __resized_slang = 2;
    signal(SIGWINCH, handler);
    if (iswaiting)
	longjmp(buf, 1);
}
#endif
static int slang_init(struct aa_context *context, int mode)
{
    if (!__slang_is_up) {
	fflush(stdout);
	SLtt_get_terminfo();
	__slang_is_up = 1;
	uninitslang = 1;
    }
    f = fopen("/dev/null", "r");
    if (SLang_init_tty(-1, 0, 0) == -1) {
	return 0;
    }
    if (-1 == SLkp_init()) {
	return 0;
    }
    __slang_keyboard = 1;
#ifdef GPM_MOUSEDRIVER
    aa_recommendlowmouse("gpm");
#endif

#ifdef SIGWINCH
    signal(SIGWINCH, handler);
#endif
    return 1;
}
static void slang_uninit(aa_context * c)
{
    if (uninitslang) {
	uninitslang = 0;
	__slang_is_up = 0;
    }
    SLang_reset_tty();
}
static int slang_getchar(aa_context * c1, int wait)
{
    int c, flag = 0;
#ifdef GPM_MOUSEDRIVER
    static Gpm_Event ev;
#endif
    struct timeval tv;
    fd_set readfds;

    if (wait) {
	setjmp(buf);
	iswaiting = 1;
    } else
	iswaiting = 0;
    if (__resized_slang == 2) {
	iswaiting = 0;
	__resized_slang = 1;
	return (AA_RESIZE);
    }
    /*non-gpm way */
    if (!wait) {
#ifdef GPM_MOUSEDRIVER
	if (gpm_fd == -1) {
#endif
	    if (!SLang_input_pending(0))
		return AA_NONE;
#ifdef GPM_MOUSEDRIVER
	} else {
	    GPM_DRAWPOINTER(&ev);
	    tv.tv_sec = 0;
	    tv.tv_usec = 0;
	    FD_ZERO(&readfds);
	    FD_SET(gpm_fd, &readfds);
	    FD_SET(STDIN_FILENO, &readfds);
	    if (!(flag = select(gpm_fd + 1, &readfds, NULL, NULL, &tv)))
		return AA_NONE;
	}
#endif
    }
#ifdef GPM_MOUSEDRIVER
    if (gpm_fd != -1) {
	GPM_DRAWPOINTER(&ev);
	while (!flag) {
	    FD_ZERO(&readfds);
	    FD_SET(gpm_fd, &readfds);
	    FD_SET(STDIN_FILENO, &readfds);
	    tv.tv_sec = 60;
	    flag = select(gpm_fd + 1, &readfds, NULL, NULL, &tv);
	}
	if (flag == -1) {
	    printf("error!\n");
	    return (AA_NONE);
	}
	if (FD_ISSET(gpm_fd, &readfds)) {
	    if (Gpm_GetEvent(&ev) && gpm_handler
		&& ((*gpm_handler) (&ev, gpm_data))) {
		gpm_hflag = 1;
		return AA_MOUSE;
	    }
	}
    }
#endif
    c = SLkp_getkey();
    iswaiting = 0;

    if (__resized_slang == 2) {
	__resized_slang = 1;
	return (AA_RESIZE);
    }
    if (c == 27)
	return (AA_ESC);
    if (c > 0 && c < 128 && c != 127)
	return (c);
    switch (c) {
    case SL_KEY_ERR:
	return (AA_NONE);
    case SL_KEY_LEFT:
	return (AA_LEFT);
    case SL_KEY_RIGHT:
	return (AA_RIGHT);
    case SL_KEY_UP:
	return (AA_UP);
    case SL_KEY_DOWN:
	return (AA_DOWN);
    case SL_KEY_BACKSPACE:
    case 127:
	return (AA_BACKSPACE);
    }
    return (AA_UNKNOWN);
}


struct aa_kbddriver kbd_slang_d =
{
    "slang", "Slang keyboard driver 1.0",
    0,
    slang_init,
    slang_uninit,
    slang_getchar,
};
#endif
