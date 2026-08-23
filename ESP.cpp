#include <imgui.h>
#include <vector>
#include "Offsets.hpp"

// 3D and 2D Vector Structure Matrices
struct Vector3 { float x, y, z; };
struct Vector2 { float x, y; };

// Function template using your custom screen transformation matrix pointer (Offset: 0x1062B69B8)
bool ProjectWorldLocationToScreen(uintptr_t basic_pointer, Vector3 WorldLocation, Vector2& ScreenLocation) {
    typedef bool (*_ProjectWorldToScreen)(uintptr_t, Vector3, Vector2&);
    return ((_ProjectWorldToScreen)(OFFSET_PROJECT_WORLD_TO_SCREEN))(basic_pointer, WorldLocation, ScreenLocation);
}

// Global Core Logic Function called safely inside CADisplayLink thread
void DrawESP() {
    if (!Settings::EnableESP) return;

    ImDrawList* drawList = ImGui::GetBackgroundDrawList();
    if (!drawList) return; // Structural safety boundary protection

    ImVec2 screenSize = ImGui::GetIO().DisplaySize;
    ImVec2 screenCenter = ImVec2(screenSize.x / 2.0f, screenSize.y / 2.0f);

    // 1. Fetching the primary World structure pointer safely
    uintptr_t UWorld = *(uintptr_t*)(OFFSET_UWORLD);
    if (!UWorld) return;

    // 2. Navigating target entity pointer lists boundary validation
    uintptr_t ActorArray = *(uintptr_t*)(UWorld + OFFSET_ACTOR_ARRAY);
    if (!ActorArray) return; // Preventing immediate crash conditions

    int ActorCount = *(int*)(UWorld + OFFSET_ACTOR_ARRAY + 0x8);
    if (ActorCount <= 0 || ActorCount > 1000) return; // Discarding unallocated corrupted frames

    // 3. Iterating through physical locations inside active chunk pools
    for (int i = 0; i < ActorCount; i++) {
        uintptr_t CurrentActor = *(uintptr_t*)(ActorArray + (i * 0x8));
        if (!CurrentActor) continue;

        // Extracting position matrix offsets securely 
        uintptr_t RootComponent = *(uintptr_t*)(CurrentActor + 0x140); 
        if (!RootComponent) continue;
        
        Vector3 PlayerWorldPos = *(Vector3*)(RootComponent + 0x1D0);

        // 4. Transforming spatial locations safely onto 2D hardware resolution
        Vector2 PlayerScreenPos;
        if (ProjectWorldLocationToScreen(UWorld, PlayerWorldPos, PlayerScreenPos)) {
            
            // Dito Design Line ESP: Screen top center straight down to coordinates
            if (Settings::DrawLines) {
                drawList->AddLine(
                    ImVec2(screenCenter.x, 0), 
                    ImVec2(PlayerScreenPos.x, PlayerScreenPos.y), 
                    ImColor(255, 0, 0, 255), // Pure Solid Red Line
                    1.5f // Precise Stroke weight
                );
            }

            // Screen space identity tracking tag layout
            if (Settings::DrawBoxes) {
                // Focus anchor indicator node point
                drawList->AddCircleFilled(ImVec2(PlayerScreenPos.x, PlayerScreenPos.y), 4.0f, ImColor(255, 255, 0, 255)); // Yellow Dot
                
                // Entity overlay text notifier element
                drawList->AddText(ImVec2(PlayerScreenPos.x + 10, PlayerScreenPos.y - 5), ImColor(255, 255, 255, 255), "Target Player");
            }
        }
    }
}
