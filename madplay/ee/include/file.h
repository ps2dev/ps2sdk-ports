#ifdef __cplusplus
extern "C" {
#endif

#define MODE_HDD 0
#define MODE_CD 1
#define MODE_MC0 2
#define MODE_MC1 3
#define MODE_HOST 4
#define MODE_MASS 5

int CheckHDD();
void setPathInfo(int argc, char **argv);
int OpenPartition(char *part);
int ClosePartition();
void closeShop(int handle);
int OpenFile(char *filename, int mode, int media);
void CloseFile(int handle, int media);
int ReadFile(int handle, unsigned char *buffer, int size, int media);
int SeekFile(int handle, int pos, int rel, int media);
void Reset();
int RunElf(char *name);
void memcpy2(unsigned char *dest,unsigned char *org,int ndata);
void memset2(unsigned char *dest,unsigned char val,int ndata);


extern int mediaMode;

#ifdef __cplusplus
}
#endif
