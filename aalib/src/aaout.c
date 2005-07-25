#include "aalib.h"
void aa_puts(aa_context * c, int x, int y, int attr, char *s)
{
    char s1[10000];
    int pos, pos1;
    int x1, y1;
    if (x < 0 || y < 0 || x >= aa_scrwidth(c) || y >= aa_scrheight(c))
	return;
    x1 = x;
    y1 = y;
    for (pos = 0; s[pos] != 0 && pos < 10000; pos++) {
	s1[pos] = s[pos];
	pos1 = x1 + y1 * aa_scrwidth(c);
	c->textbuffer[pos1] = s[pos];
	c->attrbuffer[pos1] = attr;
	x1++;
	if (x1 >= aa_scrwidth(c)) {
	    x1 = 0;
	    y1++;
	    if (y1 >= aa_scrheight(c))
		break;
	}
    }
}
void aa_resizehandler(aa_context * c, void (*handler) (aa_context *))
{
    c->resizehandler = handler;
}

void aa_hidecursor(aa_context * c)
{
    c->cursorstate--;
    if (c->cursorstate == -1 && c->driver->cursormode != NULL)
	c->driver->cursormode(c, 0);
}
void aa_showcursor(aa_context * c)
{
    c->cursorstate++;
    if (c->cursorstate == 0 && c->driver->cursormode != NULL)
	c->driver->cursormode(c, 1);
    aa_gotoxy(c, c->cursorx, c->cursory);
}
void aa_gotoxy(aa_context * c, int x, int y)
{
    if (c->cursorstate >= 0) {
	if (x < 0)
	    x = 0;
	if (y < 0)
	    y = 0;
	if (x >= aa_scrwidth(c))
	    x = aa_scrwidth(c) - 1;
	if (y >= aa_scrheight(c))
	    y = aa_scrheight(c) - 1;
	c->driver->gotoxy(c, x, y);
	c->cursorx = x;
	c->cursory = y;
    }
}
