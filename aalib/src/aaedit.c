#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include "aalib.h"
static void aa_editdisplay(struct aa_edit *e)
{
    char s[1000];
    int i;
    if (e->cursor > strlen(e->data))
	e->cursor = strlen(e->data);
    if (e->cursor < e->printpos)
	e->printpos = e->cursor;
    if (e->cursor >= e->printpos + e->size)
	e->printpos = e->cursor - e->size;
    if (e->printpos < 0)
	e->printpos = 0;
    strncpy(s, e->data + e->printpos, e->size);
    s[e->size] = 0;
    for (i = strlen(e->data) - e->printpos; i < e->size; i++)
	s[i] = ' ';
    aa_puts(e->c, e->x, e->y, e->clearafterpress ? AA_REVERSE : AA_SPECIAL, s);
    aa_gotoxy(e->c, e->x + e->cursor - e->printpos, e->y);
}
struct aa_edit *aa_createedit(aa_context * c, int x, int y, int size, char *s, int maxsize)
{
    struct aa_edit *e;
    if (x < 0)
	x = 0;
    if (y < 0)
	y = 0;
    if (x >= aa_imgwidth(c) - 1)
	x = aa_imgwidth(c) - 2;
    if (y >= aa_imgheight(c) - 1)
	y = aa_imgwidth(c) - 2;
    if (x + size >= aa_imgwidth(c))
	size = aa_imgwidth(c) - 1 - x;
    e = malloc(sizeof(struct aa_edit));
    if (e == NULL)
	return NULL;
    e->maxsize = maxsize;
    e->data = s;
    e->cursor = strlen(s);
    e->clearafterpress = 1;
    e->x = x;
    e->y = y;
    e->size = size;
    e->c = c;
    e->printpos = 0;
    aa_editdisplay(e);
    return (e);
}
static void aa_insert(struct aa_edit *e, char ch)
{
    int i, s = strlen(e->data);
    if (s == e->maxsize - 1)
	return;
    for (i = s; i >= e->cursor; i--) {
	e->data[i + 1] = e->data[i];
    }
    e->data[s + 1] = 0;
    e->data[e->cursor] = ch;
    e->cursor++;
}
static void aa_delete(struct aa_edit *e)
{
    int i, s = strlen(e->data);
    if (e->cursor == 0)
	return;
    e->cursor--;
    for (i = e->cursor; i <= s; i++) {
	e->data[i] = e->data[i + 1];
    }
}
void aa_editkey(struct aa_edit *e, int c)
{
    if (c < 127 && (isgraph(c) || c == ' ')) {
	if (e->clearafterpress)
	    e->data[0] = 0,
		e->cursor = 0;
	e->clearafterpress = 0;
	aa_insert(e, c);
	aa_editdisplay(e);
    } else if (c == AA_BACKSPACE) {
	e->clearafterpress = 0;
	aa_delete(e);
	aa_editdisplay(e);
    } else if (c == AA_LEFT) {
	e->cursor--;
	e->clearafterpress = 0;
	if (e->cursor < 0)
	    e->cursor = 0;
	aa_editdisplay(e);
    } else if (c == AA_RIGHT) {
	e->cursor++;
	e->clearafterpress = 0;
	if (e->cursor > strlen(e->data))
	    e->cursor = strlen(e->data);
	aa_editdisplay(e);
    }
    aa_flush(e->c);
}
void aa_edit(aa_context * c, int x, int y, int size, char *s, int maxsize)
{
    struct aa_edit *e;
    int ch;
    aa_showcursor(c);
    e = aa_createedit(c, x, y, size, s, maxsize);
    aa_flush(c);
    while ((ch = aa_getkey(c, 1)) != 13 && ch != '\n') {
	aa_editkey(e, ch);
    }
    aa_hidecursor(c);
    free(e);
}
