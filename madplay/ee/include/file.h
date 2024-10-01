#ifdef __cplusplus
extern "C" {
#endif

#if 0
#define MODE_HDD 0 
#define MODE_DVD 1
#define MODE_MC0 2
#define MODE_MC1 3
#define MODE_HOST 4
#define MODE_MASS 5
#endif

void setPathInfo(int argc, char **argv);
void Reset();
int RunElf(char *name);

extern int mediaMode;

#ifdef __cplusplus
}
#endif
