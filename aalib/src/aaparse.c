#include <string.h>
#include <stdlib.h>
#include "config.h"
#include "aalib.h"
static int inparse;
static void parseenv(struct aa_hardware_params *p, aa_renderparams * r);
static void aa_remove(int i, int *argc, char **argv)
{
    int y;
    if (i < 0 || i >= *argc) {
	printf("AA Internal error #1-please report\n");
	return;
    }
    for (y = i; y < *argc - 1; y++) {
	argv[y] = argv[y + 1];
    }
    argv[*argc - 1] = NULL;
    (*argc)--;
}
int aa_parseoptions(struct aa_hardware_params *p, aa_renderparams * r, int *argc, char **argv)
{
    int i, y;
    int supported;
    if (!inparse)
	parseenv(p, r);
    if (argc == NULL || argv == NULL)
	return 1;
    supported = p != NULL ? p->supported : aa_defparams.supported;
    if (p == NULL)
	p = &aa_defparams;
    if (r == NULL)
	r = &aa_defrenderparams;
    for (i = 1; i < *argc; i++) {
	if (strcmp(argv[i], "-font") == 0) {
	    aa_remove(i, argc, argv);
	    if (*argc == i) {
		fprintf(stderr, "font name expected\n");
		return (0);
	    }
	    for (y = 0; aa_fonts[y] != NULL; y++) {
		if (!strcmp(argv[i], aa_fonts[y]->name) ||
		    !strcmp(argv[i], aa_fonts[y]->shortname)) {
		    p->font = aa_fonts[y];
		    aa_remove(i, argc, argv);
		    i--;
		    break;
		}
	    }
	    if (aa_fonts[i] == NULL) {
		fprintf(stderr, "font name expected\n");
		return (0);
	    }
	    i--;
	} else if (strcmp(argv[i], "-normal") == 0) {
	    aa_remove(i, argc, argv);
	    i--;
	    supported |= AA_NORMAL_MASK;
	    p->supported = supported;
	} else if (strcmp(argv[i], "-nonormal") == 0) {
	    aa_remove(i, argc, argv);
	    i--;
	    supported &= ~AA_NORMAL_MASK;
	    p->supported = supported;
	} else if (strcmp(argv[i], "-bold") == 0) {
	    aa_remove(i, argc, argv);
	    i--;
	    supported |= AA_BOLD_MASK;
	    p->supported = supported;
	} else if (strcmp(argv[i], "-nobold") == 0) {
	    aa_remove(i, argc, argv);
	    i--;
	    supported &= ~AA_BOLD_MASK;
	    p->supported = supported;
	} else if (strcmp(argv[i], "-boldfont") == 0) {
	    aa_remove(i, argc, argv);
	    i--;
	    supported |= AA_BOLDFONT_MASK;
	    p->supported = supported;
	} else if (strcmp(argv[i], "-noboldfont") == 0) {
	    aa_remove(i, argc, argv);
	    i--;
	    supported &= ~AA_BOLDFONT_MASK;
	    p->supported = supported;
	} else if (strcmp(argv[i], "-dim") == 0) {
	    aa_remove(i, argc, argv);
	    i--;
	    supported |= AA_DIM_MASK;
	    p->supported = supported;
	} else if (strcmp(argv[i], "-nodim") == 0) {
	    aa_remove(i, argc, argv);
	    i--;
	    supported &= ~AA_DIM_MASK;
	    p->supported = supported;
	} else if (strcmp(argv[i], "-reverse") == 0) {
	    aa_remove(i, argc, argv);
	    i--;
	    supported |= AA_REVERSE_MASK;
	    p->supported = supported;
	} else if (strcmp(argv[i], "-extended") == 0) {
	    aa_remove(i, argc, argv);
	    i--;
	    supported |= AA_EXTENDED;
	    p->supported = supported;
	} else if (strcmp(argv[i], "-eight") == 0) {
	    aa_remove(i, argc, argv);
	    i--;
	    supported |= AA_EIGHT;
	    p->supported = supported;
	} else if (strcmp(argv[i], "-noreverse") == 0) {
	    aa_remove(i, argc, argv);
	    i--;
	    supported &= ~AA_REVERSE_MASK;
	    p->supported = supported;
	} else if (strcmp(argv[i], "-inverse") == 0) {
	    aa_remove(i, argc, argv);
	    i--;
	    r->inversion = 1;
	} else if (strcmp(argv[i], "-noinverse") == 0) {
	    aa_remove(i, argc, argv);
	    i--;
	    r->inversion = 0;
	} else if (strcmp(argv[i], "-nodither") == 0) {
	    aa_remove(i, argc, argv);
	    i--;
	    r->dither = 0;
	} else if (strcmp(argv[i], "-floyd_steinberg") == 0) {
	    aa_remove(i, argc, argv);
	    i--;
	    r->dither = AA_FLOYD_S;
	} else if (strcmp(argv[i], "-error_distribution") == 0) {
	    aa_remove(i, argc, argv);
	    i--;
	    r->dither = AA_ERRORDISTRIB;
	} else if (strcmp(argv[i], "-random") == 0) {
	    aa_remove(i, argc, argv);
	    if (*argc == i) {
		fprintf(stderr, "Random dithering value expected\n");
		return (0);
	    }
	    r->randomval = atol(argv[i]);
	    aa_remove(i, argc, argv);
	    i--;
	} else if (strcmp(argv[i], "-bright") == 0) {
	    aa_remove(i, argc, argv);
	    if (*argc == i) {
		fprintf(stderr, "Bright value expected(0-255)\n");
		return (0);
	    }
	    r->bright = atol(argv[i]);
	    aa_remove(i, argc, argv);
	    i--;
	} else if (strcmp(argv[i], "-contrast") == 0) {
	    aa_remove(i, argc, argv);
	    if (*argc == i) {
		fprintf(stderr, "Contrast value expected(0-255)\n");
		return (0);
	    }
	    r->contrast = atol(argv[i]);
	    aa_remove(i, argc, argv);
	    i--;
	} else if (strcmp(argv[i], "-width") == 0) {
	    aa_remove(i, argc, argv);
	    if (*argc == i) {
		fprintf(stderr, "width expected\n");
		return (0);
	    }
	    p->width = atol(argv[i]);
	    aa_remove(i, argc, argv);
	    i--;
	} else if (strcmp(argv[i], "-recwidth") == 0) {
	    aa_remove(i, argc, argv);
	    if (*argc == i) {
		fprintf(stderr, "width expected\n");
		return (0);
	    }
	    p->recwidth = atol(argv[i]);
	    aa_remove(i, argc, argv);
	    i--;
	} else if (strcmp(argv[i], "-minwidth") == 0) {
	    aa_remove(i, argc, argv);
	    if (*argc == i) {
		fprintf(stderr, "width expected\n");
		return (0);
	    }
	    p->minwidth = atol(argv[i]);
	    aa_remove(i, argc, argv);
	    i--;
	} else if (strcmp(argv[i], "-maxwidth") == 0) {
	    aa_remove(i, argc, argv);
	    if (*argc == i) {
		fprintf(stderr, "width expected\n");
		return (0);
	    }
	    p->maxwidth = atol(argv[i]);
	    aa_remove(i, argc, argv);
	    i--;
	} else if (strcmp(argv[i], "-height") == 0) {
	    aa_remove(i, argc, argv);
	    if (*argc == i) {
		fprintf(stderr, "height expected\n");
		return (0);
	    }
	    p->height = atol(argv[i]);
	    aa_remove(i, argc, argv);
	    i--;
	} else if (strcmp(argv[i], "-recheight") == 0) {
	    aa_remove(i, argc, argv);
	    if (*argc == i) {
		fprintf(stderr, "height expected\n");
		return (0);
	    }
	    p->recheight = atol(argv[i]);
	    aa_remove(i, argc, argv);
	    i--;
	} else if (strcmp(argv[i], "-minheight") == 0) {
	    aa_remove(i, argc, argv);
	    if (*argc == i) {
		fprintf(stderr, "height expected\n");
		return (0);
	    }
	    p->minheight = atol(argv[i]);
	    aa_remove(i, argc, argv);
	    i--;
	} else if (strcmp(argv[i], "-maxheight") == 0) {
	    aa_remove(i, argc, argv);
	    if (*argc == i) {
		fprintf(stderr, "height expected\n");
		return (0);
	    }
	    p->maxheight = atol(argv[i]);
	    aa_remove(i, argc, argv);
	    i--;
	} else if (strcmp(argv[i], "-gamma") == 0) {
	    aa_remove(i, argc, argv);
	    if (*argc == i) {
		fprintf(stderr, "Gamma value expected\n");
		return (0);
	    }
	    r->gamma = atof(argv[i]);
	    aa_remove(i, argc, argv);
	    i--;
	} else if (strcmp(argv[i], "-dimmul") == 0) {
	    aa_remove(i, argc, argv);
	    if (*argc == i) {
		fprintf(stderr, "Dimmul value expected\n");
		return (0);
	    }
	    p->dimmul = atof(argv[i]);
	    aa_remove(i, argc, argv);
	    i--;
	} else if (strcmp(argv[i], "-boldmul") == 0) {
	    aa_remove(i, argc, argv);
	    if (*argc == i) {
		fprintf(stderr, "Dimmul value expected\n");
		return (0);
	    }
	    p->boldmul = atof(argv[i]);
	    aa_remove(i, argc, argv);
	    i--;
	} else if (strcmp(argv[i], "-driver") == 0) {
	    aa_remove(i, argc, argv);
	    if (*argc == i) {
		fprintf(stderr, "Driver name expected\n");
		return (0);
	    }
	    aa_recommendhidisplay(argv[i]);
	    aa_remove(i, argc, argv);
	    i--;
	} else if (strcmp(argv[i], "-kbddriver") == 0) {
	    aa_remove(i, argc, argv);
	    if (*argc == i) {
		fprintf(stderr, "Driver name expected\n");
		return (0);
	    }
	    aa_recommendhikbd(argv[i]);
	    aa_remove(i, argc, argv);
	    i--;
	} else if (strcmp(argv[i], "-mousedriver") == 0) {
	    aa_remove(i, argc, argv);
	    if (*argc == i) {
		fprintf(stderr, "Driver name expected\n");
		return (0);
	    }
	    aa_recommendhimouse(argv[i]);
	    aa_remove(i, argc, argv);
	    i--;
	}
    }
    return (1);
}
static void parseenv(struct aa_hardware_params *p, aa_renderparams * r)
{
    char *env;
    int argc = 1;
    int i;
    char *argv[256], *argv1[256];
    inparse = 1;
    env = getenv("AAOPTS");
    if (env == NULL)
	return;
    if (env[0]) {
	for (i = 0; i < strlen(env) - 1; i++) {
	    int s;
	    char stop = ' ';
	    while (env[i] == ' ')
		i++;
	    if (env[i] == '"')
		i++, stop = '"';
	    s = i;
	    while (i < strlen(env) && env[i] != stop)
		i++;
	    if (i - s) {
		argv1[argc] = argv[argc] = malloc(i - s);
		strncpy(argv[argc], env + s, i - s);
		argc++;
		if (argc == 255)
		    break;
	    }
	}
    }
    i = argc;
    if (i != 1) {
	aa_parseoptions(p, r, &i, argv);
	for (i = 1; i < argc; i++)
	    free(argv1[i]);
    }
    inparse = 0;
}
