/* Hidden "this" pointer for the video functions */
#define _THIS	SDL_VideoDevice *this

typedef struct SDL_PrivateVideoData
{
	int center_x;
	int center_y;

	float ratio;
} SDL_PrivateVideoData;
