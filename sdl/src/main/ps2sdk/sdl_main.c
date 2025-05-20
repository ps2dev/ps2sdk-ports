#include <tamtypes.h>
#include <kernel.h>
#include <sifrpc.h>
#include <loadfile.h>
#include <iopheap.h>
#include <iopcontrol.h>
#include <libcdvd.h>
#include <smod.h>
#include <stdio.h>

#include "SDL_main.h"

int SDL_HasMMX()
{
	return 0;
}

#undef main
int main(int argc, char *argv[])
{
#if defined(PS2SDL_ENABLE_MTAP) && !defined(NO_IOP_HANDLING)
	SifInitRpc(0);

	smod_mod_info_t info;
	if(smod_get_mod_by_name("sio2man",&info)!=0)
	{
		printf("PS2SDL: sio2man detected, resetting iop\n");
		sceCdInit(SCECdEXIT);
		SifExitIopHeap();
		SifLoadFileExit();
		SifExitRpc();

		SifIopReset(NULL, 0);
		while (!SifIopSync()) {};
	}
#endif
	SifInitRpc(0);
	return(SDL_main(argc, argv));
}
