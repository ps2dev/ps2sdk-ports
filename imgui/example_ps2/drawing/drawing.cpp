#include "drawing.h"

void imgui_draw_circle(ImDrawList *draw_list, bool filled, const ImVec2& center, float radius, ImU32 col, int num_segments, float thickness ) {
    if (filled) {
        draw_list->AddCircleFilled(center, radius, col, num_segments);
    } else {
        draw_list->AddCircle(center, radius, col, num_segments, thickness);
    }
}

void imgui_draw_triangle(ImDrawList *draw_list, bool filled, const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, ImU32 col, float thickness) {
    if (filled) {
        draw_list->AddTriangleFilled(p1, p2, p3, col);
    } else {
        draw_list->AddTriangle(p1, p2, p3, col, thickness);
    }
}

void imgui_draw_rect(ImDrawList *draw_list, bool filled, const ImVec2& p_min, const ImVec2& p_max, ImU32 col, float rounding, ImDrawFlags flags, float thickness) {
    if (filled) {
        draw_list->AddRectFilled(p_min, p_max, col, rounding, flags);
    } else {
        draw_list->AddRect(p_min, p_max, col, rounding, flags, thickness);
    }
}

void imgui_draw_line(ImDrawList *draw_list, const ImVec2& p1, const ImVec2& p2, ImU32 col, float thickness) {
    draw_list->AddLine(p1, p2, col, thickness);
}