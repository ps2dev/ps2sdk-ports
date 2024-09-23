#include "widget.h"

void ImGui::Widgets::GamePadVisualizer(padButtonStatus *pad, float width, float height) {
    ImU16 buttons = pad->btns ^ 0xFFFF;

    const ImVec2 p = ImGui::GetCursorScreenPos();
    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    // Common radius values
    const float radius = width / 32;

    // Face buttons
    ImVec2 triangleCenter = ImVec2(p.x + width*11/14, p.y + height*3/7);
    draw_controller_triangle(draw_list, triangleCenter, radius, buttons & PAD_TRIANGLE);

    ImVec2 circleCenter = ImVec2(p.x + width*12/14, p.y + height*4/7);
    draw_controller_circle(draw_list, circleCenter, radius, buttons & PAD_CIRCLE);

    ImVec2 crossCenter = ImVec2(p.x + width*11/14, p.y + height*5/7);
    draw_controller_cross(draw_list, crossCenter, radius, buttons & PAD_CROSS);

    ImVec2 squareCenter = ImVec2(p.x + width*10/14, p.y + height*4/7);
    draw_controller_square(draw_list, squareCenter, radius, buttons & PAD_SQUARE);

    // Center buttons
    ImVec2 selectCenter = ImVec2(p.x + width*6/14, p.y + height*4/7);
    draw_controller_select(draw_list, selectCenter, radius * 0.8, buttons & PAD_SELECT);

    ImVec2 startCenter = ImVec2(p.x + width*8/14, p.y + height*4/7);
    draw_controller_start(draw_list, startCenter, radius * 0.8, buttons & PAD_START);

    // D-pad buttons
    ImVec2 dpadLeftCenter = ImVec2(p.x + width*2/14, p.y + height*4/7);
    draw_controller_dpad_left(draw_list, dpadLeftCenter, radius, buttons & PAD_LEFT);
    
    ImVec2 dpadRightCenter = ImVec2(p.x + width*4/14, p.y + height*4/7);
    draw_controller_dpad_right(draw_list, dpadRightCenter, radius, buttons & PAD_RIGHT);
    
    ImVec2 dpadUpCenter = ImVec2(p.x + width*3/14, p.y + height*3/7);
    draw_controller_dpad_up(draw_list, dpadUpCenter, radius, buttons & PAD_UP);
    
    ImVec2 dpadDownCenter = ImVec2(p.x + width*3/14, p.y + height*5/7);
    draw_controller_dpad_down(draw_list, dpadDownCenter, radius, buttons & PAD_DOWN);
    
    ImVec2 joystickLeftCenter = ImVec2(p.x + width*5.5/14, p.y + height);
    draw_controller_joystick(draw_list, joystickLeftCenter, width / 12, buttons & PAD_L3, pad->ljoy_h, pad->ljoy_v);

    ImVec2 joystickRightCenter = ImVec2(p.x + width*8.5/14, p.y + height);
    draw_controller_joystick(draw_list, joystickRightCenter, width / 12, buttons & PAD_R3, pad->rjoy_h, pad->rjoy_v);

    // Shoulder buttons
    ImVec2 shoulderL1Center = ImVec2(p.x + width*1/14, p.y + height*2.5/7);
    draw_controller_shoulder(draw_list, shoulderL1Center, width / 24, buttons & PAD_L1);
    ImVec2 shoulderL2Center = ImVec2(p.x + width*1/14, p.y + height*1.5/7);
    draw_controller_trigger(draw_list, shoulderL2Center, width / 24, buttons & PAD_L2);
    
    ImVec2 shoulderR1Center = ImVec2(p.x + width*13/14, p.y + height*2.5/7);
    draw_controller_shoulder(draw_list, shoulderR1Center, width / 24, buttons & PAD_R1);
    ImVec2 shoulderR2Center = ImVec2(p.x + width*13/14, p.y + height*1.5/7);
    draw_controller_trigger(draw_list, shoulderR2Center, width / 24, buttons & PAD_R2);

    ImGui::Dummy(ImVec2(width, height + 50));
}