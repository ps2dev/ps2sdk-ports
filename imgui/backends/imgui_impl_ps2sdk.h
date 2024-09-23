// dear imgui: Platform Backend for PlayStation 2 consoles using ps2sdk
// This needs to be used along with the PS2 gsKit renderer

// Features:
//  [ ] Platform: Gamepad support. Enabled with 'io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad'.
//  [ ] Platform: Mouse cursor shape and visibility. Disable with 'io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange'.
// 
// You can use unmodified imgui_impl_* files in your project. See examples/ folder for examples of using this.
// Prefer including the entire imgui/ repository into your project (either as a copy or as a submodule), and only build the backends you need.
// If you are new to Dear ImGui, read documentation from the docs/ folder + read the top of imgui.cpp.
// Read online: https://github.com/ocornut/imgui/tree/master/docs

#pragma once
#include "imgui.h"      // IMGUI_IMPL_API
#include <gsKit.h>
#include <libpad.h>

IMGUI_IMPL_API bool     ImGui_ImplPs2Sdk_InitForGsKit(GSGLOBAL* global);
IMGUI_IMPL_API void     ImGui_ImplPs2Sdk_Shutdown();
IMGUI_IMPL_API void     ImGui_ImplPs2Sdk_NewFrame();
