#include "drawing.h"

static const ImU32 ColorGrayLight = IM_COL32(0xC0, 0xC0, 0xC0, 0xFF);
static const ImU32 ColorGrayMedium = IM_COL32(0x60, 0x60, 0x60, 0xFF);
static const ImU32 ColorGrayDark = IM_COL32(0x20, 0x20, 0x20, 0xFF);
static const ImU32 ColorWhite = IM_COL32_WHITE;

static const ImU32 ColorTriangle = IM_COL32(0x38, 0xDE, 0xC8, 0xFF);
static const ImU32 ColorCircle = IM_COL32(0xF0, 0x6E, 0x6C, 0xFF);
static const ImU32 ColorCross = IM_COL32(0x9B, 0xAD, 0xE4, 0xFF);
static const ImU32 ColorSquare = IM_COL32(0xD5, 0x91, 0xBD, 0xFF);

static const ImU32 ColorCenterButton = ColorGrayDark;
static const ImU32 ColorCenterButtonPressed = ColorWhite;
static const ImU32 ColorDpad = ColorGrayDark;
static const ImU32 ColorDpadPressed = ColorWhite;
static const ImU32 ColorShoulder = ColorGrayDark;
static const ImU32 ColorShoulderPressed = ColorWhite;

void draw_controller_triangle(ImDrawList *draw_list, const ImVec2& center, float radius, bool pressed) {
    const float radiusSmall = radius * 0.7f;
    imgui_draw_circle(draw_list, true, center, radius, pressed ? ColorTriangle : ColorGrayDark);
    imgui_draw_triangle(draw_list, pressed, 
        ImVec2(center.x, center.y - radiusSmall),
        ImVec2(center.x + radiusSmall * cosf(-30.0f * M_PI / 180.0f), center.y - radiusSmall * sinf(-30.0f * M_PI / 180.0f)),
        ImVec2(center.x + radiusSmall * cosf(210.0f * M_PI / 180.0f), center.y - radiusSmall * sinf(210.0f * M_PI / 180.0f)),
        pressed ? ColorGrayDark : ColorTriangle);
}

void draw_controller_circle(ImDrawList *draw_list, const ImVec2& center, float radius, bool pressed) {
    const float radiusSmall = radius * 0.7f;
    imgui_draw_circle(draw_list, true, center, radius, pressed ? ColorCircle : ColorGrayDark);
    imgui_draw_circle(draw_list, pressed, center, radiusSmall, pressed ? ColorGrayDark : ColorCircle);
}

void draw_controller_cross(ImDrawList *draw_list, const ImVec2& center, float radius, bool pressed) {
    float sideHalf = 0.82f * radius * cosf(45.0f * M_PI / 180.0f);

    imgui_draw_circle(draw_list, true, center, radius, pressed ? ColorCross : ColorGrayDark);
    imgui_draw_line(draw_list,
        ImVec2(center.x - sideHalf, center.y - sideHalf),
        ImVec2(center.x + sideHalf, center.y + sideHalf),
        pressed ? ColorGrayDark : ColorCross);
    imgui_draw_line(draw_list,
        ImVec2(center.x + sideHalf, center.y - sideHalf),
        ImVec2(center.x - sideHalf, center.y + sideHalf),
        pressed ? ColorGrayDark : ColorCross);
}

void draw_controller_square(ImDrawList *draw_list, const ImVec2& center, float radius, bool pressed) {
    float sideHalf = 0.8f * radius * cosf(45.0f * M_PI / 180.0f);

    imgui_draw_circle(draw_list, true, center, radius, pressed ? ColorSquare : ColorGrayDark);
    imgui_draw_rect(draw_list, pressed,
        ImVec2(center.x - sideHalf, center.y - sideHalf),
        ImVec2(center.x + sideHalf, center.y + sideHalf),
        pressed ? ColorGrayDark : ColorSquare);
}

void draw_controller_start(ImDrawList *draw_list, const ImVec2& center, float size, bool pressed) {
    imgui_draw_triangle(draw_list, true,
        ImVec2(center.x - size, center.y - size/2),
        ImVec2(center.x - size, center.y + size/2),
        ImVec2(center.x + size, center.y),
        pressed ? ColorCenterButtonPressed : ColorCenterButton);
    imgui_draw_triangle(draw_list, false,
        ImVec2(center.x - size, center.y - size/2),
        ImVec2(center.x - size, center.y + size/2),
        ImVec2(center.x + size, center.y),
        ColorGrayMedium);
}

void draw_controller_select(ImDrawList *draw_list, const ImVec2& center, float size, bool pressed) {
    imgui_draw_rect(draw_list, true,
        ImVec2(center.x - size, center.y - size/2),
        ImVec2(center.x + size, center.y + size/2),
        pressed ? ColorCenterButtonPressed : ColorCenterButton);
    imgui_draw_rect(draw_list, false,
        ImVec2(center.x - size, center.y - size/2),
        ImVec2(center.x + size, center.y + size/2),
        ColorGrayMedium);
}

void draw_controller_dpad_left(ImDrawList *draw_list, const ImVec2& center, float size, bool pressed) {
    float sideHalf = 1.25f * size * cosf(45.0f * M_PI / 180.0f);
    float sideArrowHalf = 0.8f * size * cosf(45.0f * M_PI / 180.0f);
    float spacing = 0.5f * size;

    imgui_draw_triangle(draw_list, pressed,
        ImVec2(center.x - sideHalf/2 - spacing, center.y - sideArrowHalf),
        ImVec2(center.x - sideHalf/2 - spacing, center.y + sideArrowHalf),
        ImVec2(center.x - sideHalf/2 - spacing - sideArrowHalf, center.y),
        pressed ? ColorWhite : ColorDpad);

    imgui_draw_rect(draw_list, true,
        ImVec2(center.x - sideHalf/2, center.y - sideHalf),
        ImVec2(center.x + sideHalf/4 + sideHalf/2, center.y + sideHalf),
        pressed ? ColorDpadPressed : ColorDpad);

    imgui_draw_triangle(draw_list, true,
        ImVec2(center.x + sideHalf/4 + sideHalf/2, center.y - sideHalf + 2),
        ImVec2(center.x + sideHalf/4 + sideHalf/2, center.y + sideHalf - 1),
        ImVec2(center.x + sideHalf + sideHalf/2, center.y),
        pressed ? ColorDpadPressed : ColorDpad);
}

void draw_controller_dpad_right(ImDrawList *draw_list, const ImVec2& center, float size, bool pressed) {
    float sideHalf = 1.25f * size * cosf(45.0f * M_PI / 180.0f);
    float sideArrowHalf = 0.8f * size * cosf(45.0f * M_PI / 180.0f);
    float spacing = 0.5f * size;

    imgui_draw_triangle(draw_list, pressed,
        ImVec2(center.x + sideHalf/2 + spacing, center.y - sideArrowHalf),
        ImVec2(center.x + sideHalf/2 + spacing, center.y + sideArrowHalf),
        ImVec2(center.x + sideHalf/2 + spacing + sideArrowHalf, center.y),
        pressed ? ColorWhite : ColorDpad);

    imgui_draw_rect(draw_list, true,
        ImVec2(center.x + sideHalf/2, center.y - sideHalf),
        ImVec2(center.x - sideHalf/4 - sideHalf/2, center.y + sideHalf),
        pressed ? ColorDpadPressed : ColorDpad);

    imgui_draw_triangle(draw_list, true,
        ImVec2(center.x - sideHalf/4 - sideHalf/2, center.y - sideHalf + 2),
        ImVec2(center.x - sideHalf/4 - sideHalf/2, center.y + sideHalf - 1),
        ImVec2(center.x - sideHalf - sideHalf/2, center.y),
        pressed ? ColorDpadPressed : ColorDpad);
}

void draw_controller_dpad_up(ImDrawList *draw_list, const ImVec2& center, float size, bool pressed) {
    float sideHalf = 1.25f * size * cosf(45.0f * M_PI / 180.0f);
    float sideArrowHalf = 0.8f * size * cosf(45.0f * M_PI / 180.0f);
    float spacing = 0.5f * size;

    imgui_draw_triangle(draw_list, pressed,
        ImVec2(center.x - sideArrowHalf, center.y - sideHalf/2 - spacing),
        ImVec2(center.x + sideArrowHalf, center.y - sideHalf/2 - spacing),
        ImVec2(center.x, center.y - sideHalf/2 - spacing - sideArrowHalf),
        pressed ? ColorWhite : ColorDpad);

    imgui_draw_rect(draw_list, true,
        ImVec2(center.x - sideHalf, center.y - sideHalf/2),
        ImVec2(center.x + sideHalf, center.y + sideHalf/4 + sideHalf/2),
        pressed ? ColorDpadPressed : ColorDpad);

    imgui_draw_triangle(draw_list, true,
        ImVec2(center.x - sideHalf, center.y + sideHalf/4 + sideHalf/2),
        ImVec2(center.x + sideHalf - 1, center.y + sideHalf/4 + sideHalf/2),
        ImVec2(center.x, center.y + sideHalf + sideHalf/2),
        pressed ? ColorDpadPressed : ColorDpad);
}

void draw_controller_dpad_down(ImDrawList *draw_list, const ImVec2& center, float size, bool pressed) {
    float sideHalf = 1.25f * size * cosf(45.0f * M_PI / 180.0f);
    float sideArrowHalf = 0.8f * size * cosf(45.0f * M_PI / 180.0f);
    float spacing = 0.5f * size;

    imgui_draw_triangle(draw_list, pressed,
        ImVec2(center.x - sideArrowHalf, center.y + sideHalf/2 + spacing),
        ImVec2(center.x + sideArrowHalf, center.y + sideHalf/2 + spacing),
        ImVec2(center.x, center.y + sideHalf/2 + spacing + sideArrowHalf),
        pressed ? ColorWhite : ColorDpad);

    imgui_draw_rect(draw_list, true,
        ImVec2(center.x - sideHalf, center.y - sideHalf/4 - sideHalf/2),
        ImVec2(center.x + sideHalf, center.y + sideHalf/2),
        pressed ? ColorDpadPressed : ColorDpad);

    imgui_draw_triangle(draw_list, true,
        ImVec2(center.x - sideHalf, center.y - sideHalf/4 - sideHalf/2),
        ImVec2(center.x + sideHalf - 1, center.y - sideHalf/4 - sideHalf/2),
        ImVec2(center.x, center.y - sideHalf - sideHalf/2),
        pressed ? ColorDpadPressed : ColorDpad); 
}

void draw_controller_joystick(ImDrawList *draw_list, const ImVec2& center, float size, bool pressed, ImU8 x, ImU8 y) {
    // Draw the joystick boundaries and axes
    imgui_draw_rect(draw_list, true, ImVec2(center.x - size, center.y - size), ImVec2(center.x + size, center.y + size), pressed ? ColorWhite : ColorGrayDark, size * 0.1f);
    imgui_draw_line(draw_list, ImVec2(center.x, center.y - size), ImVec2(center.x, center.y + size - 1), pressed ? ColorGrayLight : ColorGrayMedium);
    imgui_draw_line(draw_list, ImVec2(center.x - size, center.y), ImVec2(center.x + size - 1, center.y), pressed ? ColorGrayLight : ColorGrayMedium);

    // Draw the current X and Y positions
    float horizontal = x/256.f * 2.0f - 1.0f;
    float vertical = y/256.f * 2.0f - 1.0f;
    float cursorSize = size * 0.1f;
    ImVec2 cursorCenter = ImVec2(center.x + size*horizontal, center.y + size*vertical);
    imgui_draw_line(draw_list, ImVec2(cursorCenter.x, cursorCenter.y - cursorSize), ImVec2(cursorCenter.x, cursorCenter.y + cursorSize), pressed ? ColorGrayDark : ColorWhite);
    imgui_draw_line(draw_list, ImVec2(cursorCenter.x - cursorSize, cursorCenter.y), ImVec2(cursorCenter.x + cursorSize, cursorCenter.y), pressed ? ColorGrayDark : ColorWhite);
}

void draw_controller_shoulder(ImDrawList *draw_list, const ImVec2& center, float size, bool pressed) {
    imgui_draw_rect(draw_list, true, 
        ImVec2(center.x - size, center.y - size*0.5f), 
        ImVec2(center.x + size, center.y + size*0.5f), 
        pressed ? ColorShoulderPressed : ColorShoulder,
        size * 0.1f);
}

void draw_controller_trigger(ImDrawList *draw_list, const ImVec2& center, float size, bool pressed) {
    imgui_draw_rect(draw_list, true, 
        ImVec2(center.x - size, center.y - size*0.5f), 
        ImVec2(center.x + size, center.y + size*0.75f), 
        pressed ? ColorShoulderPressed : ColorShoulder,
        size * 0.1f);

    imgui_draw_rect(draw_list, true, 
        ImVec2(center.x - size, center.y - size), 
        ImVec2(center.x + size, center.y + size*0.70f), 
        pressed ? ColorShoulderPressed : ColorShoulder,
        size * 0.75f);
}