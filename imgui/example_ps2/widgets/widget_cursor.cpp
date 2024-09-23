#include "widget.h"

void drawCustomPointerCursor() {
    ImDrawList *drawList = ImGui::GetForegroundDrawList();
    drawList->AddCircleFilled(ImGui::GetMousePos(), 10, IM_COL32(255, 255, 255, 32), 0);
    drawList->AddCircle(ImGui::GetMousePos(), 10, IM_COL32(255, 255, 255, 128), 0, 2.0f);
    drawList->AddCircle(ImGui::GetMousePos(), 6, IM_COL32(255, 255, 255, 32), 0, 2.0f);
}

void ImGui::Widgets::MouseCursor() {
    ImGuiIO& io = ImGui::GetIO();

    if (!io.MouseDrawCursor) {
        return;
    }

    // Plan to hide ImGui's native emulated mouse cursor if we are going to be painting our own cursor
    ImGuiMouseCursor newCursorValue = ImGuiMouseCursor_None;

    switch (ImGui::GetMouseCursor()) {
        case ImGuiMouseCursor_None:
        case ImGuiMouseCursor_Arrow:
            drawCustomPointerCursor();
            break;

        default:
            // Don't modify cursors that we're not overriding
            newCursorValue = ImGui::GetMouseCursor();
            break;
    }

    ImGui::SetMouseCursor(newCursorValue);
}