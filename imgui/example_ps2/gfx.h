#include <gsKit.h>
#include <tamtypes.h>

GSGLOBAL *gfx_init(bool hires, bool textureManager);
void gfx_imgui_init(GSGLOBAL *gsGlobal);
void gfx_render_clear(GSGLOBAL *gsGlobal, u64 color);
void gfx_render_begin(GSGLOBAL *gsGlobal, bool hires, bool textureManager);
void gfx_render_end(GSGLOBAL *gsGlobal, bool hires, bool textureManager, bool pixelOffset);