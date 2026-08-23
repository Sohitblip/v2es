#include <imgui.h>
#include "Offsets.hpp" // <-- Connecting with settings/offsets

void RenderMenu() {
    if (ImGui::Begin("ESP Configuration Menu", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Player Location Indicators");
        ImGui::Separator();

        // Checkbox hooks linked directly to the global settings state
        ImGui::Checkbox("Master ESP Switch", &Settings::EnableESP);
        
        if (Settings::EnableESP) {
            ImGui::Indent();
            ImGui::Checkbox("Line ESP (Screen Top to Target)", &Settings::DrawLines);
            ImGui::Checkbox("Target Location Dot", &Settings::DrawBoxes);
            ImGui::Unindent();
        }

        ImGui::End();
    }
}
