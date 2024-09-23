#include "gfx.h"

#include <imgui.h>
#include <imgui_impl_ps2sdk.h>
#include <imgui_impl_ps2gskit.h>

#include "custom_font.h"
#include "drawing/drawing.h"
#include "widgets/widget.h"

GSGLOBAL *gfx_init(bool hires, bool textureManager) {
    // TODO: Can't get hires to work on my PS2 :(
    GSGLOBAL *gsGlobal;
    int hiresPassCount;

    if (hires) {
        gsGlobal = gsKit_hires_init_global();
        gsGlobal->Mode = GS_MODE_DTV_720P;
        gsGlobal->Interlace = GS_NONINTERLACED;
        gsGlobal->Field = GS_FRAME;
        gsGlobal->Width = 1280;
        gsGlobal->Height = 720;
        hiresPassCount = 3;
    } else {
        gsGlobal = gsKit_init_global();
    }

    if ((gsGlobal->Interlace == GS_INTERLACED) && (gsGlobal->Field == GS_FRAME))
        gsGlobal->Height /= 2;

    // Setup GS global settings
    gsGlobal->PSM = GS_PSM_CT32;
    gsGlobal->PSMZ = GS_PSMZ_16S;
    gsGlobal->Dithering = GS_SETTING_ON;
    gsGlobal->DoubleBuffering = GS_SETTING_ON;
    gsGlobal->ZBuffering = GS_SETTING_ON;
    gsGlobal->PrimAlphaEnable = GS_SETTING_ON;
    gsKit_set_test(gsGlobal, GS_ZTEST_ON);
    gsKit_set_test(gsGlobal, GS_ATEST_ON);
    gsKit_set_primalpha(gsGlobal, GS_SETREG_ALPHA(0, 1, 0, 1, 128), 0);
    gsKit_set_clamp(gsGlobal, GS_CMODE_CLAMP);

    // Initialize DMA settings
    dmaKit_init(D_CTRL_RELE_OFF, D_CTRL_MFD_OFF, D_CTRL_STS_UNSPEC, D_CTRL_STD_OFF, D_CTRL_RCYC_8, 1 << DMA_CHANNEL_GIF);
    dmaKit_chan_init(DMA_CHANNEL_GIF);

    if (hires) {
        gsKit_hires_init_screen(gsGlobal, hiresPassCount);
    } else {
        gsKit_init_screen(gsGlobal);
    }

    if (textureManager) {
        gsKit_vram_clear(gsGlobal);
        gsKit_TexManager_init(gsGlobal);
    }

    return gsGlobal;
}

void gfx_imgui_init(GSGLOBAL *gsGlobal) {
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsClassic();
    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = NULL;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    io.MousePos = ImVec2(0, 0);
    io.Fonts->AddFontFromMemoryCompressedTTF(custom_font_compressed_data, custom_font_compressed_size, 16);

    ImGuiStyle& style = ImGui::GetStyle();
    style.AntiAliasedLinesUseTex = false;
    style.CellPadding = ImVec2(4, 2);
    style.ItemSpacing = ImVec2(6, 6);
    style.ItemInnerSpacing = ImVec2(4, 4);
    style.FrameRounding = 4;
    style.FramePadding = ImVec2(10, 2);
    style.ScrollbarSize = 20;
    style.GrabMinSize = 10;
    style.GrabRounding = 2;
    style.WindowBorderSize = 0;
    style.WindowRounding = 2;
    style.WindowPadding = ImVec2(6, 6);
    style.WindowTitleAlign = ImVec2(0.5, 0.5);
    style.TouchExtraPadding = ImVec2(8, 8);
    style.MouseCursorScale = 0.8;
    style.SelectableTextAlign = ImVec2(0, 0.5);

    // Setup ImGui backends
    ImGui_ImplPs2Sdk_InitForGsKit(gsGlobal);
    ImGui_ImplPs2GsKit_Init(gsGlobal);
}

void gfx_render_clear(GSGLOBAL *gsGlobal, u64 color) {
    gsGlobal->PrimAlphaEnable = GS_SETTING_OFF;
    gsKit_clear(gsGlobal, color);
    gsGlobal->PrimAlphaEnable = GS_SETTING_ON;
}

void gfx_render_begin(GSGLOBAL *gsGlobal, bool hires, bool textureManager) {
    // Start the Dear ImGui frame
    ImGui_ImplPs2Sdk_NewFrame();
    ImGui_ImplPs2GsKit_NewFrame();
    ImGui::NewFrame();
}

void gfx_render_end(GSGLOBAL *gsGlobal, bool hires, bool textureManager, bool pixelOffset) {
    // Draw our custom mouse cursor for this frame; see `widgets/widget_cursor.cpp` for 
    // examples on how to draw a custom cursor depending on the cursor type. Must be 
    // called at the end of the frame so ImGui has time to update the cursor type.
    ImGui::Widgets::MouseCursor();
    ImGui::Render();

    ImGui_ImplPs2GsKit_RenderDrawData(ImGui::GetDrawData(), pixelOffset ? ImVec2(-0.5f, -0.5f) : ImVec2(0, 0));

    if (hires) {
        gsKit_hires_sync(gsGlobal);
        gsKit_hires_flip(gsGlobal);
    } else {
        gsKit_queue_exec(gsGlobal);
        gsKit_sync_flip(gsGlobal);
    }

    if (textureManager) {
        gsKit_TexManager_nextFrame(gsGlobal);
    }
}