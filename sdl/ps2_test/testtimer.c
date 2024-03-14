#include "SDL.h"

int main(int argc, char *argv[])
{
	int now_ms, before_ms;
	int now_sec, before_sec;

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
		now_sec = now_ms / 1000;
		if (now_sec != before_sec)
		{
			printf("Time: %02d:%02d\n", now_sec / 60, now_sec % 60);
			before_sec = now_sec;
		}

		now_ms = SDL_GetTicks();
	}

	/* test 2: test SDL_Delay */
	printf("test 2: sleeping for 30 seconds with SDL_Delay\n");
	before_ms = SDL_GetTicks();
	SDL_Delay(30*1000);
	now_ms = SDL_GetTicks();
	printf("Ticks differ %d (should be 30000)\n", now_ms - before_ms);

	printf("test ended\n");
	return 0;
}
