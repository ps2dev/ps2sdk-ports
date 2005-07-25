
#include <stdio.h>
main()
{
    int i;
    while (1) {
	for (i = 0; i < 32; i++) {
	    if (i < 8)
		printf("0x%02x, ", getc(stdin));
	    else
		getc(stdin);
	    if (feof(stdin))
		exit();
	}
	printf("\n");
    }
}
