#include "widget.h"

void ImGui::Widgets::WindowOverlay(float alpha) {
    ImGui::Widgets::WindowOverlay(IM_COL32(0, 0, 0, 0xFF * alpha));
}

void ImGui::Widgets::WindowOverlay(ImU32 color) {
    ImDrawList *drawList = ImGui::GetWindowDrawList();
    ImVec2 windowPos = ImGui::GetWindowPos();
    ImVec2 windowSize = ImGui::GetWindowSize();
    drawList->AddRectFilled(windowPos, 
        ImVec2(windowPos.x + windowSize.x, windowPos.y + windowSize.y),
        color);
}