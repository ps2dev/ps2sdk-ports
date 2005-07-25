#include "aalib.h"
int main(int argc, char **argv)
{
    aa_context *c;
    int i;
    FILE *f;
    if (!aa_parseoptions(NULL, NULL, &argc, argv) || argc != 5) {
	printf("Usage: %s [options] file_name variable_name long_name short_name\n%s", argv[0], aa_help);
	exit(1);
    }
    c = aa_autoinit(&aa_defparams);
    f = fopen(argv[1], "w");
    if (f == NULL) {
	aa_close(c);
	printf("Can not open output file\n");
    }
    fprintf(f, "#include \"aalib.h\"\n"
	    "static unsigned char %sdata[] =\n"
	    "{", argv[2]);
    for (i = 0; i < c->params.font->height * 256; i++) {
	if (!(i % c->params.font->height))
	    fprintf(f, "\n\t");
	fprintf(f, "0x%02x, ", c->params.font->data[i]);
    }
    fprintf(f, "};\n\n"
	    "struct aa_font %s =\n"
	    "\t{%sdata, %i, \"%s\", \"%s\"};\n", argv[2], argv[2], c->params.font->height, argv[3], argv[4]);
    aa_close(c);
    return 1;
}
