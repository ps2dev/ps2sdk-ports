#include <tamtypes.h>
#include <sifrpc.h>
#include "SDL_main.h"

int SDL_HasMMX() 
{ 
	return 0; 
}

#undef main
int main(int argc, char *argv[])
{
	SifInitRpc(0); 
	return(SDL_main(argc, argv));
}
