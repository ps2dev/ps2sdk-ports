#pragma once

#include "../drawing/drawing.h"

#include <libpad.h>
#include <imgui.h>


namespace ImGui::Widgets {
    enum WidgetGamePadIconType {
        WidgetGamePadIconType_Triangle,
        WidgetGamePadIconType_Circle,
        WidgetGamePadIconType_Cross,
        WidgetGamePadIconType_Square,
        WidgetGamePadIconType_Start,
        WidgetGamePadIconType_Select,
        WidgetGamePadIconType_DpadUp,
        WidgetGamePadIconType_DpadDown,
        WidgetGamePadIconType_DpadLeft,
        WidgetGamePadIconType_DpadRight,
    };

    void GamePadIcon(WidgetGamePadIconType icon, float height = 0);
    void GamePadVisualizer(padButtonStatus *pad, float width = 300.f, float height = 150.f);
    void WindowOverlay(float alpha = 0.75);
    void WindowOverlay(ImU32 color);
    void MouseCursor();
}