#include "aalib.h"
int aa_initkbd(struct aa_context *c, struct aa_kbddriver *d, int mode)
{
    if (d->init(c, mode)) {
	c->kbddriver = d;
	return 1;
    }
    return 0;
}
int aa_initmouse(struct aa_context *c, struct aa_mousedriver *d, int mode)
{
    if (d->init(c, mode)) {
	c->mousedriver = d;
	c->mousemode=1;
	return 1;
    }
    return 0;
}
void aa_getmouse(struct aa_context *c, int *x, int *y, int *z)
{
    *x = 0;
    *y = 0;
    *z = 0;
    if (c->mousedriver) {
	c->mousedriver->getmouse(c, x, y, z);
    }
}
int aa_getevent(aa_context * c, int wait)
{
    int x, y, b;
    int ch;
    if (c->mousedriver != NULL) {
	c->mousedriver->getmouse(c, &x, &y, &b);
	if (x != c->mousex || y != c->mousey || b != c->buttons) {
	    c->mousex = x;
	    c->mousey = y;
	    c->buttons = b;
	    return (AA_MOUSE);
	}
    }
    if (c->kbddriver == NULL)
	return (AA_UNKNOWN);
    if (wait) {
	while ((ch = c->kbddriver->getkey(c, 1)) == AA_NONE) {
	    if (c->mousedriver != NULL) {
		c->mousedriver->getmouse(c, &x, &y, &b);
		if (x != c->mousex || y != c->mousey || b != c->buttons) {
		    c->mousex = x;
		    c->mousey = y;
		    c->buttons = b;
		    return (AA_MOUSE);
		}
	    }
	}
    } else
	ch = c->kbddriver->getkey(c, 0);
    if (ch == AA_RESIZE && c->resizehandler != NULL)
	c->resizehandler(c);
    if (ch == AA_MOUSE) {
	if (c->mousedriver != NULL) {
	    c->mousedriver->getmouse(c, &x, &y, &b);
	    if (x != c->mousex || y != c->mousey || b != c->buttons) {
		c->mousex = x;
		c->mousey = y;
		c->buttons = b;
		return (AA_MOUSE);
	    } else
		return (aa_getevent(c, wait));
	} else
	    return (AA_UNKNOWN);
    }
    return (ch);
}
int aa_getkey(aa_context * co, int wait)
{
    int c;
    do {
	c = aa_getevent(co, wait);
    } while (c == AA_MOUSE || c == AA_RESIZE || c>=AA_RELEASE);
    return (c);
}
