#pragma once

#include <imgui.h>
#include <cmath>

#ifndef M_PI
#define M_PI		3.14159265358979323846
#endif

void imgui_draw_circle(ImDrawList *draw_list, bool filled, const ImVec2& center, float radius, ImU32 col, int num_segments = 0, float thickness = 1.0f);
void imgui_draw_triangle(ImDrawList *draw_list, bool filled, const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, ImU32 col, float thickness = 1.0f);
void imgui_draw_rect(ImDrawList *draw_list, bool filled, const ImVec2& p_min, const ImVec2& p_max, ImU32 col, float rounding = 0.0f, ImDrawFlags flags = 0, float thickness = 1.0f);
void imgui_draw_line(ImDrawList *draw_list, const ImVec2& p1, const ImVec2& p2, ImU32 col, float thickness = 1.0f);

void draw_controller_triangle(ImDrawList *draw_list, const ImVec2& center, float radius, bool pressed);
void draw_controller_circle(ImDrawList *draw_list, const ImVec2& center, float radius, bool pressed);
void draw_controller_cross(ImDrawList *draw_list, const ImVec2& center, float radius, bool pressed);
void draw_controller_square(ImDrawList *draw_list, const ImVec2& center, float radius, bool pressed);
void draw_controller_start(ImDrawList *draw_list, const ImVec2& center, float size, bool pressed);
void draw_controller_select(ImDrawList *draw_list, const ImVec2& center, float size, bool pressed);
void draw_controller_dpad_left(ImDrawList *draw_list, const ImVec2& center, float size, bool pressed);
void draw_controller_dpad_right(ImDrawList *draw_list, const ImVec2& center, float size, bool pressed);
void draw_controller_dpad_up(ImDrawList *draw_list, const ImVec2& center, float size, bool pressed);
void draw_controller_dpad_down(ImDrawList *draw_list, const ImVec2& center, float size, bool pressed);
void draw_controller_joystick(ImDrawList *draw_list, const ImVec2& center, float size, bool pressed, ImU8 x, ImU8 y);
void draw_controller_shoulder(ImDrawList *draw_list, const ImVec2& center, float size, bool pressed);
void draw_controller_trigger(ImDrawList *draw_list, const ImVec2& center, float size, bool pressed);