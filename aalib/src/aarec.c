#include <string.h>
#include <malloc.h>
#include "aalib.h"
aa_linkedlist *aa_kbdrecommended = NULL, *aa_mouserecommended = NULL,
*aa_displayrecommended = NULL;
#define addlist(l,m) ((((l)!=NULL)?(m)->next=(l),(m)->previous=(l)->previous,(l)->previous=(m),(m)->previous->next=(m):((m)->next=(m),(m)->previous=(m),(l)=(m))))
#define inslist(l,m) (addlist(l,m),(l)=(m))
#define remove(l,m) ((m)->next->previous=(m)->previous,(m)->previous->next=(m)->next,((l)==m?(l)=((l)->next==(l)?NULL:(l)->next):NULL))
static aa_linkedlist *aa_find(aa_linkedlist * l, char *text)
{
    aa_linkedlist *m = l;
    if (l == NULL)
	return NULL;
    do {
	if (!strcmp(m->text, text))
	    return (m);
	m = m->next;
    } while (l != m);
    return NULL;
}
void aa_recommendhi(aa_linkedlist ** l, char *name)
{
    aa_linkedlist *m = (aa_linkedlist *) malloc(sizeof(*m)), *o = aa_find(*l, name);
    if (o != NULL)
	remove(*l, o);
    m->text = strdup(name);
    inslist(*l, m);
}

void aa_recommendlow(aa_linkedlist ** l, char *name)
{
    aa_linkedlist *o = aa_find(*l, name);
    if (o == NULL) {
	aa_linkedlist *m = (aa_linkedlist *) malloc(sizeof(*m));
	m->text = strdup(name);
	addlist(*l, m);
    }
}

char *aa_getfirst(aa_linkedlist ** l)
{
    char *c = NULL;
    aa_linkedlist *m = *l;
    if (*l != NULL) {
	remove(*l, m);
	c = m->text;
	free((char *) m);
    }
    return (c);
}
#if 0
main()
{
    char *t;
    aa_linkedlist *l = NULL;
    aa_recommendhi(&l, "ahoj1");
    aa_recommendlow(&l, "ble1");
    aa_recommendhi(&l, "ahoj2");
    aa_recommendlow(&l, "ble2");
    aa_recommendlow(&l, "ble1");
    aa_recommendhi(&l, "ble1");
    while ((t = aa_getfirst(&l)) != NULL) {
	printf("-%s-\n", t);
    }
}
#endif
