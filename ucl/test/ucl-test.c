#include <stdio.h>
#include <ucl/ucl.h>

void n2e_decompress(const void * source, void * dest);

int main(void) {
    FILE * f;
    int size_in, size_out;
    char * in, * out, * in2;
    
    f = fopen("host:ucl-test.c", "rb");
    fseek(f, 0, SEEK_END);
    size_in = ftell(f);
    size_out = size_in * 1.2 + 2048;
    fseek(f, 0, SEEK_SET);
    
    in = (char *) malloc(size_in);
    in2 = (char *) malloc(size_in);
    out = (char *) malloc(size_out);
    
    fread(in, size_in, 1, f);
    
    printf("Loaded %i bytes.\n", size_in);
    
    if (ucl_nrv2e_99_compress(in, size_in, out, &size_out, NULL, 10, NULL, NULL) != UCL_E_OK) {
        printf("Error during ucl_nrv2b_99_compress.\n");
	SleepThread();
    }
    
    printf("Packed to %i bytes.\nUnpacking...", size_out);
    
    memset(in2, 0, size_in);
    
    n2e_decompress(out, in2);
    
    printf("Done, comparing.\n");
    
    if (memcmp(in, in2, size_in)) {
	printf("Different!\n");
    } else {
	printf("Identical!\n");
    }
    
    
    SleepThread();    
}
