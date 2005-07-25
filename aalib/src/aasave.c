#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "aalib.h"

#define NORMALPRINTS { "%s", "%s", "%s", "%s", "%s", }
#define NONE { "", "", "", "", "", }

static char *html_escapes[] =
{"<", "&lt;", ">", "&gt;", "&", "&amp;", NULL};
static char *html_alt_escapes[] =
{"<", "&lt;", ">", "&gt;", "&", "&amp;", "\"", "&quot;", NULL};
#ifdef VYHEN_SUPPORT
static char *vyhen_escapes[] =
{"|", "||","~", "~~", NULL};
#endif

static char *irc_escapes[] =
{"@", "@@", NULL};

static char *generate_filename(char *template, char *result, int x, int y, int pages, char *extension);

struct aa_format aa_nhtml_format =
{
    79, 36,
    79, 36,
    0,
    AA_NORMAL_MASK | AA_BOLD_MASK | AA_BOLDFONT_MASK | AA_DIM_MASK,
    &aa_fontcourier,
    "Nestcapeized html",
    ".html",
    "<HTML>\n<HEAD><TITLE>Ascii arted image done using aalib</TITLE>\n</HEAD>\n<BODY BGCOLOR=\"#000000\" TEXT=\"#b2b2b2\" LINK=\"#FFFFFF\">\n<FONT COLOR=#b2b2b2 SIZE=2><PRE>\n",
    "</PRE></FONT></BODY>\n</HTML>\n",
    "\n",
    NORMALPRINTS,
    {"",
     "<FONT COLOR=\"686868\">",
     "<FONT COLOR=\"ffffff\">",
     "",
     "<B>"
    },
    {"",
     "</FONT>",
     "</FONT>",
     "",
     "</B>"
    },
    html_escapes
};
struct aa_format aa_html_format =
{
    79, 25,
    79, 25,
    0,
    AA_NORMAL_MASK | AA_BOLD_MASK | AA_BOLDFONT_MASK,
    NULL,
    "Pure html",
    ".html",
    "<HTML>\n <HEAD> <TITLE>Ascii arted image done using aalib</TITLE>\n</HEAD>\n<BODY><PRE>\n",
    "</PRE></BODY>\n</HTML>\n",
    "\n",
    NORMALPRINTS,
    {"",
     "",
     "<B>",
     "",
     "<B>"
    },
    {"",
     "",
     "</B>",
     "",
     "</B>"
    },
    html_escapes
};
struct aa_format aa_ansi_format =
{
    80, 25,
    80, 25,
    0,
    AA_NORMAL_MASK | AA_BOLD_MASK | AA_BOLDFONT_MASK | AA_REVERSE_MASK | AA_DIM_MASK,
    NULL,
    "ANSI escape seqences",
    ".ansi",
    "",
    "",
    "\n",
    NORMALPRINTS,
    {"",
     "\33[8m",
     "\33[1m",
     "\33[1m",
     "\33[7m"
    },
    {"",
     "\33[0;10m",
     "\33[0;10m",
     "\33[0;10m",
     "\33[0;10m",
    },
    NULL
};
#ifdef VYHEN_SUPPORT
struct aa_format aa_vyhen_format =
{
    80, 30,
    80, 30,
    0,
    AA_NORMAL_MASK | AA_BOLD_MASK | AA_REVERSE_MASK | AA_DIM_MASK | AA_EXTENDED,
    NULL,
    "vyhen",
    ".vyhen",
    "",
    "",
    "\n",
    NORMALPRINTS,
    {"|07",
     "|08",
     "|15",
     "||7|00",
     "||1|15"
    },
    {"",
     "",
     "",
     "||9",
     "||0",
    },
    vyhen_escapes
};
#endif
struct aa_format aa_text_format =
{
    80, 25,
    80, 25,
    0,
    AA_NORMAL_MASK,
    NULL,
    "Text file",
    ".txt",
    "",
    "",
    "\n",
    NORMALPRINTS,
    NONE,
    NONE,
    NULL
};
struct aa_format aa_more_format =
{
    80, 25,
    80, 25,
    AA_NORMAL_SPACES,
    AA_NORMAL_MASK | AA_BOLD_MASK | AA_BOLDFONT_MASK,
    NULL,
    "For more/less",
    ".cat",
    "",
    "",
    "\n",
		/* Can leave these %c iff there are no conversions to multi-character
		 * thingies. */
    {"%s", "%s", "%s\10%s", "%s\10%s", "%s"},
    NONE,
    NONE,
    NULL
};
struct aa_format aa_hp_format =
{
    130 * 2, 64 * 2 - 1,
    130, 64 * 2 - 1,
    AA_USE_PAGES,
    AA_NORMAL_MASK,
    &aa_fontline,
    "HP laser jet - A4 small font",
    ".hp",
    "\33(10U\33(s0p16.67h8.5v0s0b0T\33%%0A\33&/0U\33&/0Z\33&/24D",
    "\14",
    "\r\33=",
    NORMALPRINTS,
    NONE,
    NONE,
    NULL
};
struct aa_format aa_hp2_format =
{
    80 * 2, 64 - 1,
    80, 64 - 1,
    AA_USE_PAGES,
    AA_NORMAL_MASK,
    &aa_font16,
    "HP laser jet - A4 big font",
    ".hp",
    "\33(s7B\33*p0Y@",
    "\14",
    "\r\n",
    NORMALPRINTS,
    NONE,
    NONE,
    NULL
};
struct aa_format aa_irc_format =
{
    70, 25,
    70, 25,
    0,
    AA_NORMAL_MASK | AA_BOLD_MASK | AA_REVERSE_MASK,
    &aa_font16,
    "For catting to an IRC channel",
    ".irc",
    "",
    "",
    "\n",
    NORMALPRINTS,
    {"",
     "",
     "",
     "",
     ""
    },
    {"",
     "",
     "",
     "",
     ""
    },
    NULL
};
struct aa_format aa_zephyr_format =
{
    70, 25,
    70, 25,
    0,
    AA_NORMAL_MASK | AA_DIM_MASK | AA_BOLD_MASK,
    &aa_font16,
    "For catting to an IRC channel II",
    ".irc",
    "",
    "",
    "\n",
    NORMALPRINTS,
    {"",
     "@color(gray50)",
     "@b(",
     "",
     ""
    },
    {"",
     "@color(black)",
     ")",
     "",
     ""
    },
    irc_escapes
};
struct aa_format aa_roff_format =
{
    70, 25,
    70, 25,
    0,
    AA_NORMAL_MASK | AA_BOLD_MASK,
    &aa_font16,
    "For including in a man page",
    ".man",
    "",
    "",
    "\n.br\n",
    NORMALPRINTS,
    {"",
     "",
     "\n.B ",
     "",
     ""
    },
    {"",
     "",
     "\n",
     "",
     ""
    },
    irc_escapes
};

struct aa_format aa_html_alt_format =
{
    79, 25,
    79, 25,
    0,
    AA_NORMAL_MASK,
    NULL,
    "HTML <IMG ALT= tag",
    ".html",
		/* Need that newline, because lynx inserts a space, thus shifting
		 * the first line of our art. */
    "<PRE><IMG SRC=\"your image here\" ALT=\"\n",
    "\"></PRE>\n",
    "\n",
    NORMALPRINTS,
    {"",
     "",
     "",
     "",
     ""
    },
    {"",
     "",
     "",
     "",
     ""
    },
    html_alt_escapes
};

struct aa_format *aa_formats[] =
{
#ifdef VYHEN_SUPPORT
    &aa_vyhen_format,
#endif
    &aa_text_format,
    &aa_html_format,
    &aa_nhtml_format,
    &aa_html_alt_format,
    &aa_more_format,
    &aa_ansi_format,
    &aa_hp_format,
    &aa_hp2_format,
    &aa_irc_format,
    &aa_zephyr_format,
    &aa_roff_format,
    NULL
};

struct aa_driver save_d;
#define FORMAT ((struct aa_savedata *)c->driverdata)->format
#define DATA ((struct aa_savedata *)c->driverdata)

static char **build_conversions(char **in, char **conv)
{
    char c;

    memset(conv, 0, sizeof(char *) * 256);

    if (in != NULL)
	while (*in != NULL && *(in + 1) != NULL) {
	    c = **in;
	    in++;
	    conv[c] = *in;
	    in++;
	}
    return conv;
}

static int save_init(struct aa_hardware_params *p, void *none,
		     struct aa_hardware_params *dest, void **data)
{
    struct aa_savedata *d = (struct aa_savedata *) none;
    static struct aa_hardware_params def;
    *data = (void *) malloc(sizeof(struct aa_savedata));
    memcpy(*data, d, sizeof(struct aa_savedata));
    *dest = def;
    if (p->font == NULL)
	dest->font = d->format->font;
    dest->width = d->format->width;
    dest->height = d->format->height;
    dest->supported = d->format->supported;


    return 1;
}
static void save_uninit(aa_context * c)
{
}
static void save_getsize(aa_context * c, int *width, int *height)
{
}
static void save_gotoxy(aa_context * c, int x, int y)
{
}

static int lastattr;
static FILE *f;
static aa_context *c;
static void stop_tag()
{
    if (lastattr != -1)
	fputs(FORMAT->ends[lastattr], f);
    lastattr = -1;
}
static void start_tag(int attr)
{
    if (attr > AA_NATTRS)
	attr = AA_NATTRS;
    lastattr = attr;
    fputs(FORMAT->begin[lastattr], f);
}
static void encodechar(unsigned char attr, unsigned char ch, char **conversions)
{
    char chr[2];
    if (FORMAT->flags & AA_NORMAL_SPACES && ch == ' ' && (attr != AA_REVERSE))
	attr = AA_NORMAL;
    if (attr != lastattr) {
	stop_tag();
	start_tag(attr);
    }
    if (conversions[ch] == NULL) {
	chr[0] = ch;
	chr[1] = 0;
	fprintf(f, FORMAT->prints[attr], chr, chr, chr, chr);
    } else {
	fprintf(f, FORMAT->prints[attr], conversions[ch], conversions[ch],
		conversions[ch], conversions[ch]);
    }
}

static void savearea(int x1, int y1, int x2, int y2, char **conversions)
{
    int x, y;
    fputs(FORMAT->head, f);
    lastattr = -1;
    for (y = y1; y < y2; y++) {
	for (x = x1; x < x2; x++) {
	    if (x < 0 || x >= aa_scrwidth(c) ||
		y < 0 || y >= aa_scrheight(c))
		encodechar(AA_NORMAL, ' ', conversions);
	    else {
		int pos = x + y * aa_scrwidth(c);
		encodechar(c->attrbuffer[pos], c->textbuffer[pos], conversions);
	    }
	}
	stop_tag();
	fputs(FORMAT->newline, f);
    }
    fputs(FORMAT->end, f);
    fflush(f);
}

static void save_flush(aa_context * c1)
{
    char fname[4096];
    char *conversions[256];
    c = c1;
    build_conversions(FORMAT->conversions, conversions);
    if (FORMAT->flags & AA_USE_PAGES) {
	int xpages = (aa_scrwidth(c1) + FORMAT->pagewidth - 1) /
	FORMAT->pagewidth;
	int ypages = (aa_scrheight(c1) + FORMAT->pageheight - 1) /
	FORMAT->pageheight;
	int x, y;
	for (x = 0; x < xpages; x++)
	    for (y = 0; y < ypages; y++) {
		if (DATA->name != NULL) {
		    generate_filename(DATA->name, fname, x, y, 1, FORMAT->extension);
		    f = fopen(fname, "w");
		} else
		    f = DATA->file;

		if (f == NULL)
		    return;
		savearea(x * FORMAT->pagewidth, y * FORMAT->pageheight,
			 (x + 1) * FORMAT->pagewidth,
			 (y + 1) * FORMAT->pageheight, conversions);
		if (DATA->name != NULL)
		    fclose(f);
	    }

    } else {
	if (DATA->name != NULL) {
	    generate_filename(DATA->name, fname, 0, 0, 0, FORMAT->extension);
	    f = fopen(fname, "w");
	} else
	    f = DATA->file;
	if (f == NULL)
	    return;
	savearea(0, 0, /*DATA->width, DATA->height */ aa_scrwidth(c1), aa_scrheight(c1), conversions);
	if (DATA->name != NULL)
	    fclose(f);
    }
}

/*
 * Takes a template of the form blah%xfu%y.extension and will return
 * either blah<x>fu<y>.extension or blahfu.extension depending on whether
 * pages is true or not. %x and %y may be flipped, but may appear only once.
 * If you want a % in the output, you'll have to use %%.
 * Using %[^%xy] is not defined. The results are also undefined if you don't
 * include both %x and %y.
 *
 * Rewrote by Jan Hubicka to accept unlimited (and zero) number of % stuff
 * and %e for extension and %c for coordinates in format %x_%y.
 * You might now add after filename simply %c%e to get filename
 * blahX_Y.txt or blah.txt
 * Also avoided memory owerflows. 
 */
static char *generate_filename(char *template, char *result, int x, int y, int pages, char *extension)
{
    char *a;
    char *b;
    char *end = result + 4090;
    b = template - 1, a = result - 1;
    while ((*++a = *++b)) {
	if (a >= end)
	    break;		/*Too long filename - probably bug in caller, 
				   since filenames longer than 4KB are useless :) */
	if (*b == '%') {
	    switch (b[1]) {
	    case 'x':
		a--;
		if (pages) {
		    char text[8];
		    char *e = text - 1;
		    sprintf(text, "%i", x);
		    while ((*++a = *++e))
			if (a >= end)
			    break;
		    a--;
		}
		b++;
		break;
	    case 'y':
		a--;
		if (pages) {
		    char text[8];
		    char *e = text - 1;
		    sprintf(text, "%i", y);
		    while ((*++a = *++e))
			if (a >= end)
			    break;
		    a--;
		}
		b++;
		break;
	    case 'c':
		a--;
		if (pages) {
		    char text[8];
		    char *e = text - 1;
		    sprintf(text, "_%i_%i", x, y);
		    while ((*++a = *++e))
			if (a >= end)
			    break;
		    a--;
		}
		b++;
		break;
	    case 'e':
		a--;
		{
		    char *e = extension - 1;
		    while ((*++a = *++e))
			if (a >= end)
			    break;
		    a--;
		}
		b++;
		break;
	    case '%':
		a--;
		b++;
		break;
	    }
	    if (!*b)
		break;
	}
    }
    *a = 0;
    return result;
}

struct aa_driver save_d =
{
    "save", "Special driver for saving to files",
    save_init,
    save_uninit,
    save_getsize,
    NULL,
    NULL,
    save_gotoxy,
    save_flush,
};
