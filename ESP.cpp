#include <imgui.h>
#include <vector>
#include <cmath>
#include "Offsets.hpp" // <-- Connecting to fetch world pointers and screen matrix

struct Vector3 { float x, y, z; };
struct Vector2 { float x, y; };

// Function mapping for the 3D-to-2D coordinates calculation 
bool ProjectWorldLocationToScreen(uintptr_t basic_pointer, Vector3 WorldLocation, Vector2& ScreenLocation) {
    typedef bool (*_ProjectWorldToScreen)(uintptr_t, Vector3, Vector2&);
    return ((_ProjectWorldToScreen)(OFFSET_PROJECT_WORLD_TO_SCREEN))(basic_pointer, WorldLocation, ScreenLocation);
}

// Global drawing hook loop
void DrawESP() {
    if (!Settings::EnableESP) return;

    ImDrawList* drawList = ImGui::GetBackgroundDrawList();
    ImVec2 screenSize = ImGui::GetIO().DisplaySize;
    ImVec2 screenCenter = ImVec2(screenSize.x / 2.0f, screenSize.y / 2.0f);

    // Fetch the structural core of the engine base pointer
    uintptr_t UWorld = *(uintptr_t*)(OFFSET_UWORLD);
    if (!UWorld) return;

    uintptr_t ActorArray = *(uintptr_t*)(UWorld + OFFSET_ACTOR_ARRAY);
    int ActorCount = *(int*)(UWorld + OFFSET_ACTOR_ARRAY + 0x8);

    // Loop through entities inside the active view matrix chunk
    for (int i = 0; i < ActorCount; i++) {
        uintptr_t CurrentActor = *(uintptr_t*)(ActorArray + (i * 0x8));
        if (!CurrentActor) continue;

        // Position processing matrix layers
        uintptr_t RootComponent = *(uintptr_t*)(CurrentActor + 0x140); 
        if (!RootComponent) continue;
        
        Vector3 PlayerWorldPos = *(Vector3*)(RootComponent + 0x1D0);

        Vector2 PlayerScreenPos;
        if (ProjectWorldLocationToScreen(UWorld, PlayerWorldPos, PlayerScreenPos)) {
            
            // Draw Line ESP from screen top down to target destination 
            if (Settings::DrawLines) {
                drawList->AddLine(
                    ImVec2(screenCenter.x, 0), 
                    ImVec2(PlayerScreenPos.x, PlayerScreenPos.y), 
                    ImColor(255, 0, 0, 255), // Solid red hue line
                    1.5f
                );
            }

            // Draw target tracking node dot and notification string text
            if (Settings::DrawBoxes) {
                drawList->AddCircleFilled(ImVec2(PlayerScreenPos.x, PlayerScreenPos.y), 4.0f, ImColor(255, 255, 0, 255));
                drawList->AddText(ImVec2(PlayerScreenPos.x + 10, PlayerScreenPos.y - 5), ImColor(255, 255, 255, 255), "Target Player");
            }
        }
    }
}
