#include <malloc.h>
#include "config.h"
#include "aalib.h"
#include <string.h>
struct aa_driver *aa_drivers[] =
{
#ifdef DJGPP
    &dos_d,
#else
#ifdef X11_DRIVER
    &X11_d,
#endif
#ifdef LINUX_DRIVER
//    &linux_d,
#endif
#ifdef SLANG_DRIVER
    &slang_d,
#endif
#ifdef CURSES_DRIVER
    &curses_d,
#endif
#ifdef OS2_DRIVER
    &os2vio_d,
#endif
#endif
    &stdout_d,
    &stderr_d,
    NULL
};
aa_context *aa_autoinit(struct aa_hardware_params *params)
{
    aa_context *context = NULL;
    int i = 0;
    char *t;
    while ((t = aa_getfirst(&aa_displayrecommended)) != NULL) {
	if (context == NULL) {
	    for (i = 0; aa_drivers[i] != NULL; i++) {
		if (!strcmp(t, aa_drivers[i]->name) || !strcmp(t, aa_drivers[i]->shortname)) {
		    context = aa_init(aa_drivers[i], params, NULL);
		    break;
		}
	    }
	    if (aa_drivers[i] == NULL)
		printf("Driver %s unknown", t);
	    free(t);
	}
    }
    i = 0;
    while (context == NULL) {
	if (aa_drivers[i] == NULL)
	    return NULL;
	context = aa_init(aa_drivers[i], params, NULL);
	i++;
    }
    return (context);
}
