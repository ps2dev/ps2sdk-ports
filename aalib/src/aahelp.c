#include "config.h"
#include "aalib.h"
char *aa_help =
"  -driver        select driver\n"
"                  available drivers:"
#ifndef __DJGPP__
#ifdef LINUX_DRIVER
"linux "
#endif
#ifdef SLANG_DRIVER
"slang "
#endif
#ifdef CURSES_DRIVER
"curses "
#endif
#ifdef X11_DRIVER
"X11 "
#endif
#ifdef OS2_DRIVER
"os2 "
#endif
#else
"dos "
#endif
"stdout stderr\n"
"  -kbddriver     select keyboard driver\n"
"                  available drivers:"
#ifndef __DJGPP__
#ifdef SLANG_KBDDRIVER
"slang "
#endif
#ifdef CURSES_KBDDRIVER
"curses "
#endif
#ifdef X11_KBDDRIVER
"X11 "
#endif
#ifdef OS2_DRIVER
"os2 "
#endif
#else
"dos "
#endif
"stdin\n"
"  -mousedriver     select mouse driver\n"
"                  available drivers:"
#ifndef __DJGPP
#ifdef X11_MOUSEDRIVER
"X11 "
#endif
#ifdef OS2_DRIVER
"os2 "
#endif
#ifdef GPM_MOUSEDRIVER
"gpm "
#endif
#ifdef CURSES_MOUSEDRIVER
"curses"
#endif
"dos "
#endif
"\n\n"
"Size options:\n"
"  -width         set width\n"
"  -height        set height\n"
"  -minwidth      set minimal width\n"
"  -minheight     set minimal height\n"
"  -maxwidth      set maximal width\n"
"  -maxheight     set maximal height\n"
"  -recwidth      set recomended width\n"
"  -recheight     set recomended height\n\n"
"Attributes:\n"
"  -dim           enable usage of dim (half bright) attribute\n"
"  -bold          enable usage of bold (double bright) attribute\n"
"  -reverse       enable usage of reverse attribute\n"
"  -normal        enable usage of normal attribute\n"
"  -boldfont      enable usage of boldfont attrubute\n"
"  -no<attr>      disable (i.e -nobold)\n\n"
"Font rendering options:\n"
"  -extended      use all 256 characters\n"
"  -eight         use eight bit ascii\n"
"  -font <font>   select font(This option have effect just on hardwares\n"
"                  where aalib is unable to determine current font\n"
"                  available fonts:vga8 vga9 mda14 vga14 X8x13 X8x16 \n"
"                  X8x13bold vgagl8 line\n\n"
"Rendering options:\n"
"  -inverse       enable inverse rendering\n"
"  -noinverse     disable inverse rendering\n"
"  -bright <val>  set bright (0-255)\n"
"  -contrast <val>set contrast (0-255)\n"
"  -gamma <val>   set gamma correction value(0-1)\n\n"
"Ditherng options:\n"
"  -nodither      disable dithering\n"
"  -floyd_steinberg floyd steinberg dithering\n"
"  -error_distribution error distribution dithering\n"
"  -random <val>  set random dithering value(0-inf)\n"
"Monitor parameters:\n"
"  -dimmul <val>  multiply factor for dim color (5.3)\n"
"  -boldmul <val> multiply factor for dim color (2.7)\n"
"  The default parameters are set to fit my monitor (15\" goldstar)\n"
"  With contrast set to maximum and bright set to make black black\n"
"  This values depends at quality of your monitor (and setting of controls\n"
"  Defaultd settings should be OK for most PC monitors. But ideal monitor\n"
"  Needs dimmul=1.71 boldmul=1.43. For example monitor used by SGI is very\n"
"  close to this values. Also old 14\" vga monitors needs higher values.\n";
