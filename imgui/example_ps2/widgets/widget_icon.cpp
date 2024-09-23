#include "widget.h"

void ImGui::Widgets::GamePadIcon(WidgetGamePadIconType icon, float height) {
    const float radius = height > 0 ? height/2 : ImGui::GetFrameHeight()/2.5;
    const ImVec2 p = ImGui::GetCursorScreenPos();
    const ImVec2 center(p.x + radius, p.y + radius);
    ImDrawList *drawList = ImGui::GetWindowDrawList();
    ImGui::Dummy(ImVec2(radius * 2, radius * 2));

    switch (icon) {
        case WidgetGamePadIconType_Triangle:    draw_controller_triangle(drawList, center, radius, false); break;
        case WidgetGamePadIconType_Circle:      draw_controller_circle(drawList, center, radius, false); break;
        case WidgetGamePadIconType_Cross:       draw_controller_cross(drawList, center, radius, false); break;
        case WidgetGamePadIconType_Square:      draw_controller_square(drawList, center, radius, false); break;
        case WidgetGamePadIconType_Start:       draw_controller_start(drawList, center, radius, false); break;
        case WidgetGamePadIconType_Select:      draw_controller_select(drawList, center, radius, false); break;
        case WidgetGamePadIconType_DpadUp:      draw_controller_dpad_up(drawList, center, radius, false); break;
        case WidgetGamePadIconType_DpadDown:    draw_controller_dpad_down(drawList, center, radius, false); break;
        case WidgetGamePadIconType_DpadLeft:    draw_controller_dpad_left(drawList, center, radius, false); break;
        case WidgetGamePadIconType_DpadRight:   draw_controller_dpad_right(drawList, center, radius, false); break;
    }
}