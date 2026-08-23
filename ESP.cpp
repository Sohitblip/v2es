#include <imgui.h>
#include <vector>
#include <cmath>
#include "Offsets.hpp"

// Setup underlying system types
struct Vector3 { float x, y, z; };
struct Vector2 { float x, y; };

extern void RenderMenu(); // Linking with Menu.cpp

bool ProjectWorldLocationToScreen(uintptr_t basic_pointer, Vector3 WorldLocation, Vector2& ScreenLocation) {
    typedef bool (*_ProjectWorldToScreen)(uintptr_t, Vector3, Vector2&);
    return ((_ProjectWorldToScreen)(OFFSET_PROJECT_WORLD_TO_SCREEN))(basic_pointer, WorldLocation, ScreenLocation);
}

void DrawESP() {
    if (!Settings::EnableESP) return;

    ImDrawList* drawList = ImGui::GetBackgroundDrawList();
    ImVec2 screenSize = ImGui::GetIO().DisplaySize;
    ImVec2 screenCenter = ImVec2(screenSize.x / 2.0f, screenSize.y / 2.0f);

    uintptr_t UWorld = *(uintptr_t*)(OFFSET_UWORLD);
    if (!UWorld) return;

    uintptr_t ActorArray = *(uintptr_t*)(UWorld + OFFSET_ACTOR_ARRAY);
    int ActorCount = *(int*)(UWorld + OFFSET_ACTOR_ARRAY + 0x8);

    for (int i = 0; i < ActorCount; i++) {
        uintptr_t CurrentActor = *(uintptr_t*)(ActorArray + (i * 0x8));
        if (!CurrentActor) continue;

        uintptr_t RootComponent = *(uintptr_t*)(CurrentActor + 0x140); 
        if (!RootComponent) continue;
        
        Vector3 PlayerWorldPos = *(Vector3*)(RootComponent + 0x1D0);

        Vector2 PlayerScreenPos;
        if (ProjectWorldLocationToScreen(UWorld, PlayerWorldPos, PlayerScreenPos)) {
            
            if (Settings::DrawLines) {
                drawList->AddLine(
                    ImVec2(screenCenter.x, 0), 
                    ImVec2(PlayerScreenPos.x, PlayerScreenPos.y), 
                    ImColor(255, 0, 0, 255), 
                    1.5f
                );
            }

            if (Settings::DrawBoxes) {
                drawList->AddCircleFilled(ImVec2(PlayerScreenPos.x, PlayerScreenPos.y), 4.0f, ImColor(255, 255, 0, 255));
                drawList->AddText(ImVec2(PlayerScreenPos.x + 10, PlayerScreenPos.y - 5), ImColor(255, 255, 255, 255), "Target Player");
            }
        }
    }
}

// Global Core Hook for iOS Frame Integration 
// This creates the standard overlay loop on top of the mobile canvas
void HackRenderLoop() {
    // Start the tracking frame state
    ImGui::NewFrame();
    
    // Always render menu frame overlay
    RenderMenu();
    
    // Process world coordinates mapping
    DrawESP();
    
    // Finalize layout matrix buffer
    ImGui::Render();
}
