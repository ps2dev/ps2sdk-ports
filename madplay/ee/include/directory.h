#ifdef __cplusplus
extern "C" {
#endif

struct object
{
	char name[255];
	char type;
	char flag;
	int  count;
};

struct folder
{
	int iDir;
	int fIndex;
	int fMax;
	char full[512];
	char directory[255];
	//char oldDirectory[255];
	struct object object[255];
};

int openDirectory(char *dir, int media);
int closeDirectory(int media);
int changeDirectory(char *dir, int media);
int readDirectory(char *ext, int media);
int closeDirectory(int media);
int currentType();
char *currentName();
int incrementDirectory();
int decrementDirectory();
int currentIndex();
int maxObjects();
char *currentFullName();
void sortFolder();
char *getCurrentDirectory();
void resetDirectory(char *dir);

int fillObjectInfo(int current, int max, struct object *obj[]);

#ifdef __cplusplus
}
#endif
