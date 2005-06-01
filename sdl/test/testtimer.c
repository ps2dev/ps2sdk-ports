#include "SDL.h"

int main(int argc, char *argv[])
{
	int now_ms, before_sec;

	if (SDL_Init(SDL_INIT_TIMER) < 0)
	{
		fprintf(stderr, "failed to initialize SDL\n");
		return 1;
	}

	atexit(SDL_Quit);

	/* test 1: print pseudo timer since SDL start */
	printf("test 1: 60 seconds timer\n");
	before_sec = -1;
	now_ms = SDL_GetTicks();
	while (now_ms < 60*1000)
	{
		int now_sec = now_ms / 1000;
		if (now_sec != before_sec)
		{
			printf("Time: %02d:%02d\n", now_sec / 60, now_sec % 60);
			before_sec = now_sec;
		}

		now_ms = SDL_GetTicks();
	}

	printf("test ended\n");
	return 0;
}
