#include <kernel.h>
#include <loadfile.h>
#include <sifrpc.h>
#include <stdio.h>

#include "custom_font.h"
#include "drawing/drawing.h"
#include "gfx.h"
#include "pad.h"
#include "widgets/widget.h"

void demo_widget_controls_table(const char *id) {
    ImGui::BeginTable(id, 2,
                      ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingFixedFit);
    {
        ImGui::TableSetupColumn("Controls");
        ImGui::TableSetupColumn("Function");
        ImGui::TableHeadersRow();

        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::Text("D-Pad");
        ImGui::TableNextColumn();
        ImGui::Text("Navigate / Modify values");

        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::Text("L-Joystick");
        ImGui::TableNextColumn();
        ImGui::Text("Scroll window");

        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::Text("R-Joystick");
        ImGui::TableNextColumn();
        ImGui::Text("Move cursor");

        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::Text("R2 / R3");
        ImGui::TableNextColumn();
        ImGui::Text("Click with cursor");

        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::Widgets::GamePadIcon(ImGui::Widgets::WidgetGamePadIconType_Triangle);
        ImGui::SameLine();
        ImGui::Text("+ D-Pad");
        ImGui::TableNextColumn();
        ImGui::Text("Resize window");

        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::Widgets::GamePadIcon(ImGui::Widgets::WidgetGamePadIconType_Triangle);
        ImGui::SameLine();
        ImGui::Text("+ L-Joystick");
        ImGui::TableNextColumn();
        ImGui::Text("Move window");

        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::Widgets::GamePadIcon(ImGui::Widgets::WidgetGamePadIconType_Triangle);
        ImGui::SameLine();
        ImGui::Text("+ L1/R1");
        ImGui::TableNextColumn();
        ImGui::Text("Focus windows");
    }
    ImGui::EndTable();
}

void demo_paned(const ImGuiIO &io, PadStatus *pad_status, bool use_pixel_offset) {
    // Whether the user is selecting a section in the left pane
    static bool is_selecting_section = false;

    // Actively displayed section content
    static int selected_content = 0;

    // Full screen "window" that can't be moved or resized
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(io.DisplaySize);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 10));
    ImGui::Begin("PS2 + ImGui", NULL,
                 ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
    {
        // PushStyleVar in previous lines was used to increase the title-bar height
        ImGui::PopStyleVar();

        if (is_selecting_section) {
            // Only allow the left pane to have focus while the user selects
            ImGui::SetNextWindowFocus();
        }
        ImGui::BeginChild("List", ImVec2(150, 0), true);
        {
            if (ImGui::Selectable("Introduction", !is_selecting_section && selected_content == 0, 0, ImVec2(0, 25))) {
                selected_content = 0;
                is_selecting_section = false;
            }
            if (ImGui::Selectable("Gamepad", !is_selecting_section && selected_content == 1, 0, ImVec2(0, 25))) {
                selected_content = 1;
                is_selecting_section = false;
            }
            if (ImGui::Selectable("Style Editor", !is_selecting_section && selected_content == 2, 0, ImVec2(0, 25))) {
                selected_content = 2;
                is_selecting_section = false;
            }
        }
        ImGui::EndChild();

        ImGui::SameLine();
        ImGui::BeginGroup();
        {
            // Content pane will retain focus if it is selected
            if (selected_content >= 0 && !is_selecting_section) {
                ImGui::SetNextWindowFocus();
            }

            // Content pane has full height, less the height of a single frame which will be used to
            // display contextual button controls
            ImGui::BeginChild("ChildSection", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()), true);
            {
                if (selected_content == 0) {
                    // Introduction section
                    ImGui::TextColored(ImVec4(0.4, 0.6, 1.0, 1.0), "Welcome to the PS2 ImGui demo!");
                    ImGui::Separator();

                    ImGui::Text("Press");
                    ImGui::SameLine();
                    ImGui::Widgets::GamePadIcon(ImGui::Widgets::WidgetGamePadIconType_Triangle);
                    ImGui::SameLine();
                    ImGui::TextWrapped("and use the D-pad to change demo sections.");
                    ImGui::Separator();

                    ImGui::Text("Press");
                    ImGui::SameLine();
                    ImGui::Widgets::GamePadIcon(ImGui::Widgets::WidgetGamePadIconType_Start);
                    ImGui::SameLine();
                    ImGui::TextWrapped("START to switch to the windowed demo.");
                    ImGui::Separator();

                    demo_widget_controls_table("PanedControlsTable");
                }

                if (selected_content == 1) {
                    // Gamepad section
                    ImGui::Text("Custom drawn widget!");
                    ImGui::Separator();
                    ImGui::Widgets::GamePadVisualizer(&pad_status->pad, ImGui::GetWindowWidth() * 0.95,
                                                      ImGui::GetWindowHeight() * 0.50);
                }

                if (selected_content == 2) {
                    // Style editor section (using ImGui's built-in editor window)
                    ImGui::ShowStyleEditor();
                }

                // Allow the user to change sections using the triangle button
                if (pad_status->buttonsNew & PAD_TRIANGLE) {
                    is_selecting_section = true;
                }

                // Dim this content section window if the user is selecting a different section
                if (is_selecting_section) {
                    ImGui::Widgets::WindowOverlay(0.6f);
                }
            }
            ImGui::EndChild();

            // Bottom row with control information
            {
                ImGui::Widgets::GamePadIcon(ImGui::Widgets::WidgetGamePadIconType_Triangle);
                ImGui::SameLine();
                ImGui::Text("Change Section");
                ImGui::SameLine();

                ImGui::Widgets::GamePadIcon(ImGui::Widgets::WidgetGamePadIconType_Start);
                ImGui::SameLine();
                ImGui::Text("Change Demo");
                ImGui::SameLine();

                ImGui::Widgets::GamePadIcon(ImGui::Widgets::WidgetGamePadIconType_Select);
                ImGui::SameLine();
                ImGui::Text(use_pixel_offset ? "Disable Pixel Offset" : "Enable Pixel Offset");
                ImGui::SameLine();
            }
        }
        ImGui::EndGroup();
    }
    ImGui::End();
}

void demo_windowed(const ImGuiIO &io, PadStatus *pad_status, bool use_pixel_offset) {
    int spacing = 10;

    ImGui::SetNextWindowPos(ImVec2(spacing, spacing), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x / 2 - 1.5 * spacing, io.DisplaySize.y / 2 - 1.5 * spacing),
                             ImGuiCond_FirstUseEver);
    ImGui::Begin("PS2 + ImGui (Windowed)", NULL, ImGuiWindowFlags_NoCollapse);
    {
        ImGui::SetWindowFontScale(0.9f);
        ImGui::Text("Press");
        ImGui::SameLine();
        ImGui::Widgets::GamePadIcon(ImGui::Widgets::WidgetGamePadIconType_Start);
        ImGui::SameLine();
        ImGui::Text("START to change demo types.");
        ImGui::Separator();
        demo_widget_controls_table("WindowedControlsTable");
        ImGui::SetWindowFontScale(1.0f);
    }
    ImGui::End();

    // Draw the controller
    ImGui::SetNextWindowPos(ImVec2(spacing, io.DisplaySize.y / 2 + spacing / 2), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x / 2 - 1.5 * spacing, io.DisplaySize.y / 2 - 1.5 * spacing),
                             ImGuiCond_FirstUseEver);
    ImGui::Begin("Gamepad", NULL, ImGuiWindowFlags_NoCollapse);
    {
        ImGui::Text("Custom drawn widget!");
        ImGui::Separator();
        ImGui::Widgets::GamePadVisualizer(&pad_status->pad, ImGui::GetWindowWidth() * 0.95,
                                          ImGui::GetWindowHeight() * 0.45);
    }
    ImGui::End();

    // Draw the style editor
    ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x / 2 + 0.5 * spacing, spacing), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x / 2 - 1.5 * spacing, io.DisplaySize.y - 2 * spacing),
                             ImGuiCond_FirstUseEver);
    ImGui::Begin("Style Editor", NULL, ImGuiWindowFlags_NoCollapse);
    ImGui::ShowStyleEditor();
    ImGui::End();
}

static void load_modules(void) {
    int ret;

    SifInitRpc(0);

    ret = SifLoadModule("rom0:SIO2MAN", 0, NULL);
    if (ret < 0) {
        printf("sifLoadModule sio failed: %d\n", ret);
    }

    ret = SifLoadModule("rom0:PADMAN", 0, NULL);
    if (ret < 0) {
        printf("sifLoadModule pad failed: %d\n", ret);
    }
}

int main(int argc, char **argv) {
    load_modules();
    pad_init();

    // Setup the graphics and ImGui systems
    bool hires = false;
    bool use_texture_manager = true;
    GSGLOBAL *global = gfx_init(hires, use_texture_manager);
    gfx_imgui_init(global);
    ImGuiIO &io = ImGui::GetIO();

    // Rendering data!
    PadStatus pad_status;
    bool use_paned_sample = true;
    bool use_pixel_offset = true;

    while (1) {
        gfx_render_begin(global, hires, use_texture_manager);
        gfx_render_clear(global, GS_SETREG_RGBA(0x30, 0x30, 0x40, 0x80));
        pad_get_status(&pad_status);

        if (use_paned_sample) {
            demo_paned(io, &pad_status, use_pixel_offset);
        } else {
            demo_windowed(io, &pad_status, use_pixel_offset);
        }

        // Switch between demo modes if the user clicks the start button
        if (pad_status.buttonsNew & PAD_START) {
            use_paned_sample = !use_paned_sample;
        }

        // Switch between pixel offsets modes if the user clicks the select button
        if (pad_status.buttonsNew & PAD_SELECT) {
            use_pixel_offset = !use_pixel_offset;
        }

        gfx_render_end(global, hires, use_texture_manager, use_pixel_offset);
    }

    return 0;
}
