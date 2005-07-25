#include "config.h"
#include <string.h>
#include <stdio.h>
#include <signal.h>
#include <setjmp.h>
#include <sys/time.h>
#include <sys/types.h>
#ifdef GPM_MOUSEDRIVER
#include <gpm.h>
#endif
#include "aalib.h"
static int iswaiting;
static int __resized;
#ifdef GPM_MOUSEDRIVER
extern int __curses_usegpm;
#endif
static jmp_buf buf;
#ifdef SIGWINCH
static void handler(int i)
{
    __resized = 2;
    signal(SIGWINCH, handler);
    if (iswaiting)
	longjmp(buf, 1);
}
#endif
static int stdin_init(struct aa_context *context, int mode)
{
#ifdef SIGWINCH
    signal(SIGWINCH, handler);
#endif
#ifdef GPM_MOUSEDRIVER
    aa_recommendlowmouse("gpm");
#endif
    return 1;
}
static void stdin_uninit(aa_context * c)
{
#ifdef SIGWINCH
    signal(SIGWINCH, SIG_IGN);	/*this line may cause problem... */
#endif
}
static int stdin_getchar(aa_context * c1, int wait)
{
    int c;
    int flag;
    struct timeval tv;

    if (wait) {
	setjmp(buf);
	iswaiting = 1;
    }
    if (__resized == 2) {
	__resized = 1;
	return (AA_RESIZE);
    }
    if (!wait) {
	fd_set readfds;
	tv.tv_sec = 0;
	tv.tv_usec = 0;
	FD_ZERO(&readfds);
	FD_SET(0, &readfds);
#ifdef GPM_MOUSEDRIVER
	if (__curses_usegpm) {
	    FD_SET(gpm_fd, &readfds);
	}
#endif
#ifdef GPM_MOUSEDRIVER
	if (!(flag = select((__curses_usegpm ? gpm_fd : 0) + 1, &readfds, NULL, NULL, &tv)))
#else
	if (!(flag = select(1, &readfds, NULL, NULL, &tv)))
#endif
	    return AA_NONE;

    }
#ifdef GPM_MOUSEDRIVER
    if (__curses_usegpm) {
	c = Gpm_Getc(stdin);
    } else
#endif
	c = getc(stdin);
    iswaiting = 0;
    if (c == 27)
	return (AA_ESC);
    if (c == 10)
	return (13);
    if (c > 0 && c < 127 && c != 127)
	return (c);
    switch (c) {
#ifdef KEY_MOUDE
    case KEY_MOUSE:
	return AA_MOUSE
#endif
    case 127:
	return (AA_BACKSPACE);
    }
    if(feof(stdin)) return AA_NONE;
    return (AA_UNKNOWN);
}


struct aa_kbddriver kbd_stdin_d =
{
    "stdin", "Standard input keyboard driver 1.0",
    0,
    stdin_init,
    stdin_uninit,
    stdin_getchar,
};
