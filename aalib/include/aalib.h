#ifndef __AALIB_INCLUDED__
#define __AALIB_INCLUDED__
#include <stdio.h>
#ifdef __cplusplus
extern "C" {
#endif
#define AA_LIB_VERSION 1
#define AA_LIB_MINNOR 2
#define AA_LIB_VERSIONCODE 102000

#define AA_NATTRS 5
#define AA_NPARAMS 5

#define AA_NONE 0		/*special keycodes */
#define AA_RESIZE 258
#define AA_MOUSE 259
#define AA_UP 300
#define AA_DOWN 301
#define AA_LEFT 302
#define AA_RIGHT 303
#define AA_BACKSPACE 304
#define AA_ESC 305
#define AA_UNKNOWN 400
#define AA_RELEASE 65536


#define AA_NORMAL_MASK 1	/*masks for attributes */
#define AA_DIM_MASK 2
#define AA_BOLD_MASK 4
#define AA_BOLDFONT_MASK 8
#define AA_REVERSE_MASK 16
#define AA_ALL 128
#define AA_EIGHT 256
#define AA_EXTENDED (AA_ALL|AA_EIGHT)

#define AA_NORMAL 0		/*attribute numbers */
#define AA_DIM 1
#define AA_BOLD 2
#define AA_BOLDFONT 3
#define AA_REVERSE 4
#define AA_SPECIAL 5

#define AA_ERRORDISTRIB 1	/*dithering types + AA_NONE */
#define AA_FLOYD_S 2
#define AA_DITHERTYPES 3

#define AA_BUTTON1 1		/*mouse */
#define AA_BUTTON2 2
#define AA_BUTTON3 4
#define AA_MOUSEMOVEMASK 1
#define AA_MOUSEPRESSMASK 2
#define AA_PRESSEDMOVEMASK 4
#define AA_MOUSEALLMASK 7
#define AA_HIDECURSOR 8

#define AA_SENDRELEASE 1
#define AA_KBDALLMASK 1

#define AA_USE_PAGES 1		/*save format flags */
#define AA_NORMAL_SPACES 8


struct aa_hardware_params {
    struct aa_font *font;
    int supported;
    int minwidth, minheight;
    int maxwidth, maxheight;
    int recwidth, recheight;
    int mmwidth, mmheight;
    int width, height;
    double dimmul, boldmul;
};

struct aa_context {
    struct aa_driver *driver;
    struct aa_kbddriver *kbddriver;
    struct aa_mousedriver *mousedriver;
    struct aa_hardware_params params,driverparams;
    int mulx, muly;
    int imgwidth, imgheight;
    unsigned char *imagebuffer;
    unsigned char *textbuffer;
    unsigned char *attrbuffer;
    unsigned short *table;
    unsigned short *filltable;
    struct parameters *parameters;
    int cursorx, cursory, cursorstate;
    int mousex, mousey, buttons,mousemode;
    void (*resizehandler) (struct aa_context *);
    void *driverdata;
    void *kbddriverdata;
    void *mousedriverdata;
};

struct aa_driver {
    char *shortname, *name;
    int (*init) (struct aa_hardware_params *, void *, struct aa_hardware_params *,void **);
    void (*uninit) (struct aa_context *);
    void (*getsize) (struct aa_context *, int *, int *);
    void (*setattr) (struct aa_context *, int);
    void (*print) (struct aa_context *, char *);
    void (*gotoxy) (struct aa_context *, int, int);
    void (*flush) (struct aa_context *);
    void (*cursormode) (struct aa_context *, int);
};

struct aa_kbddriver {
    char *shortname, *name;
    int flags;
    int (*init) (struct aa_context *, int mode);
    void (*uninit) (struct aa_context *);
    int (*getkey) (struct aa_context *, int);
};

struct aa_mousedriver {
    char *shortname, *name;
    int flags;
    int (*init) (struct aa_context *, int mode);
    void (*uninit) (struct aa_context *);
    void (*getmouse) (struct aa_context *, int *, int *, int *);
    void (*cursormode) (struct aa_context *,int);
};

struct aa_renderparams {
    int bright, contrast;
    float gamma;
    int dither;
    int inversion;
    int randomval;
};

struct aa_edit {
    int maxsize;
    char *data;
    int cursor;
    int clearafterpress;
    int printpos;
    int x, y, size;
    struct aa_context *c;
};

struct parameters {
    unsigned int p[AA_NPARAMS];
};

struct aa_font {
    unsigned char *data;
    int height;
    char *name;
    char *shortname;
};

struct aa_format {
    int width, height;
    int pagewidth, pageheight;
    int flags;
    int supported;
    struct aa_font *font;
    char *formatname;
    char *extension;
    /*fields after this line may change in future versions*/
    char *head;
    char *end;
    char *newline;
    char *prints[AA_NATTRS];
    char *begin[AA_NATTRS];
    char *ends[AA_NATTRS];
    char **conversions;	
};

struct aa_savedata {
    char *name;
    struct aa_format *format;
    FILE *file; /*You might specify filename by this way too, in case name is NULL*/
};

struct aa_linkedlist {
    char *text;
    struct aa_linkedlist *next, *previous;
};

typedef struct aa_context aa_context;
typedef struct aa_linkedlist aa_linkedlist;
typedef struct aa_linkedlist aa_reclist;
typedef struct aa_renderparams aa_renderparams;
typedef struct aa_hardwareparams aa_hardware_params;
typedef struct aa_driver aa_driver;
typedef struct aa_font aa_font;
typedef struct aa_format aa_format;
typedef struct aa_savedata aa_savedata;
typedef int aa_palette[256];

extern struct aa_driver save_d, mem_d;
extern char *aa_help;
extern struct aa_format aa_htmlk_format, *aa_formats[];
extern struct aa_format aa_nhtml_format, aa_html_format, aa_html_alt_format,
	aa_ansi_format, aa_text_format, aa_more_format, aa_hp_format,
	aa_hp2_format, aa_irc_format, aa_zephyr_format;
extern struct aa_font *aa_fonts[];
extern int nfonts;
extern struct aa_font aa_font8, aa_font14, aa_font16, aa_font9, aa_fontline, aa_fontgl,
 aa_fontX13, aa_fontX16, aa_fontX13B, aa_fontcourier, aa_fontvyhen;
extern char *aa_dithernames[];
extern struct aa_hardware_params aa_defparams;
extern struct aa_renderparams aa_defrenderparams;
extern struct aa_driver *aa_drivers[], curses_d, dos_d, linux_d, slang_d,
 stdout_d, stderr_d, X11_d, os2vio_d;
extern struct aa_kbddriver *aa_kbddrivers[], kbd_curses_d, kbd_slang_d,
 kbd_stdin_d, kbd_dos_d, kbd_X11_d, kbd_os2_d;
extern struct aa_mousedriver *aa_mousedrivers[], mouse_curses_d, mouse_gpm_d,
 mouse_X11_d, mouse_dos_d, mouse_os2_d;
extern aa_linkedlist *aa_kbdrecommended, *aa_mouserecommended, *aa_displayrecommended;


#define aa_callnonull(c,x) ((x)!=NULL?((x)(c)):0)
#define aa_scrwidth(a) ((a)->params.width)
#define aa_scrheight(a) ((a)->params.height)
#define aa_mmwidth(a) ((a)->params.mmwidth)
#define aa_mmheight(a) ((a)->params.mmheight)
#define aa_imgwidth(a) ((a)->imgwidth)
#define aa_imgheight(a) ((a)->imgheight)
#define aa_putpixel(c,x,y,color) ((c)->imagebuffer[(x)+(y)*(aa_imgwidth(c))]=(color))
#define aa_image(c) ((c)->imagebuffer)
#define aa_text(c) ((c)->textbuffer)
#define aa_attrs(c) ((c)->attrbuffer)
#define aa_setpalette(palette,index,r,g,b) ((palette)[index]=(((r)*30+(g)*59+(b)*11)>>8))
#define aa_recommendhikbd(t) aa_recommendhi(&aa_kbdrecommended,t);
#define aa_recommendhimouse(t) aa_recommendhi(&aa_mouserecommended,t);
#define aa_recommendhidisplay(t) aa_recommendhi(&aa_displayrecommended,t);
#define aa_recommendlowkbd(t) aa_recommendlow(&aa_kbdrecommended,t);
#define aa_recommendlowmouse(t) aa_recommendlow(&aa_mouserecommended,t);
#define aa_recommendlowdisplay(t) aa_recommendlow(&aa_displayrecommended,t);

/*automatical init functions */
aa_context *aa_autoinit(struct aa_hardware_params *params);
int aa_autoinitkbd(struct aa_context *context, int mode);
int aa_autoinitmouse(struct aa_context *c, int mode);
void aa_recommendhi(aa_linkedlist ** l, char *name);
void aa_recommendlow(aa_linkedlist ** l, char *name);
char *aa_getfirst(aa_linkedlist ** l);

/*init functions */
aa_context *aa_init(struct aa_driver *driver, struct aa_hardware_params *defparams, void *driverdata);
int aa_initkbd(struct aa_context *context, struct aa_kbddriver *drv, int mode);
int aa_initmouse(struct aa_context *c, struct aa_mousedriver *d, int mode);

/*uninicializing functions */
void aa_close(aa_context * c);
void aa_uninitkbd(struct aa_context *context);
void aa_uninitmouse(struct aa_context *context);

/*rendering functions */
void aa_fastrender(aa_context * c, int x1, int y1, int x2, int y2);
void aa_render(aa_context * c, aa_renderparams * p, int x1, int y1, int x2, int y2);
void aa_renderpalette(aa_context * c, aa_palette table, aa_renderparams * p, int x1, int y1, int x2, int y2);
aa_renderparams *aa_getrenderparams(void);
void aa_flush(aa_context * c);

/*text i/o functions */
void aa_puts(aa_context * c, int x, int y, int attr, char *s);
int aa_printf(aa_context *c, int x, int y, int attr, char *fmt, ...);
void aa_gotoxy(aa_context * c, int x, int y);
void aa_hidecursor(aa_context * c);
void aa_showcursor(aa_context * c);
void aa_getmouse(aa_context * c, int *x, int *y, int *b);
void aa_hidemouse(aa_context *c);
void aa_showmouse(aa_context *c);

/*hardware parameters control */
int aa_registerfont(struct aa_font *f);
void aa_setsupported(aa_context * c, int supported);
void aa_setfont(aa_context * c, struct aa_font *font);

/*keyboard functions */
int aa_getevent(aa_context * c, int wait);
int aa_getkey(aa_context * c, int wait);

/*resize functions */
int aa_resize(aa_context * c);
void aa_resizehandler(aa_context * c, void (*handler) (aa_context *));

/*utility */
int aa_parseoptions(struct aa_hardware_params *p, aa_renderparams * r, int *argc, char **argv);
void aa_edit(aa_context * c, int x, int y, int size, char *s, int maxsize);
struct aa_edit *aa_createedit(aa_context * c, int x, int y, int size, char *s, int maxsize);
void aa_editkey(struct aa_edit *e, int c);

#ifdef __cplusplus
}
#endif
#endif
