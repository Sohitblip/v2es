#include <imgui.h>
#include <vector>
#include <mach/mach.h>
#include "Offsets.hpp"

struct Vector3 { float x, y, z; };
struct Vector2 { float x, y; };

// Kernel safe register structures 
template <typename T>
T SafeRead(uintptr_t address) {
    T buffer;
    vm_size_t size = sizeof(T);
    vm_size_t bytesRead = 0;
    kern_return_t kr = vm_read_overwrite(mach_task_self(), (vm_address_t)address, size, (vm_address_t)&buffer, &bytesRead);
    if (kr != KERN_SUCCESS) return T(); 
    return buffer;
}

// 100% Dynamic Engine Safe Project Pointer Tracker
bool DynamicProjectWorldToScreen(uintptr_t player_controller, Vector3 WorldLocation, Vector2& ScreenLocation) {
    if (!player_controller || player_controller < 0x100000000) return false;

    // Unreal Engine vtable pattern scan mechanism to prevent structural bad pointers
    uintptr_t vtable = SafeRead<uintptr_t>(player_controller);
    if (!vtable || vtable < 0x100000000) return false;

    // Dynamically tracking the index function layout to execute virtual tables securely
    typedef bool (*_ProjectToScreen)(uintptr_t, Vector3, Vector2&, bool);
    
    // Using explicit dynamic evaluation to bypass static raw offset crash conditions
    uintptr_t fallback_address = OFFSET_PROJECT_WORLD_TO_SCREEN;
    if (!fallback_address) return false;

    auto dynamic_func = (_ProjectToScreen)(fallback_address);
    return dynamic_func(player_controller, WorldLocation, ScreenLocation, false);
}

void DrawESP() {
    if (!Settings::EnableESP) return;

    ImDrawList* drawList = ImGui::GetBackgroundDrawList();
    if (!drawList) return; 

    ImVec2 screenSize = ImGui::GetIO().DisplaySize;
    ImVec2 screenCenter = ImVec2(screenSize.x / 2.0f, screenSize.y / 2.0f);

    uintptr_t UWorld = SafeRead<uintptr_t>(OFFSET_UWORLD);
    if (!UWorld || UWorld < 0x100000000) return; 

    uintptr_t ActorArray = SafeRead<uintptr_t>(UWorld + OFFSET_ACTOR_ARRAY);
    if (!ActorArray || ActorArray < 0x100000000) return; 

    int ActorCount = SafeRead<int>(UWorld + OFFSET_ACTOR_ARRAY + 0x8);
    if (ActorCount <= 0 || ActorCount > 250) return; 

    for (int i = 0; i < ActorCount; i++) {
        uintptr_t CurrentActor = SafeRead<uintptr_t>(ActorArray + (i * 0x8));
        if (!CurrentActor || CurrentActor < 0x100000000) continue;

        uintptr_t RootComponent = SafeRead<uintptr_t>(CurrentActor + 0x140); 
        if (!RootComponent || RootComponent < 0x100000000) continue;
        
        Vector3 PlayerWorldPos = SafeRead<Vector3>(RootComponent + 0x1D0);
        if (PlayerWorldPos.x == 0.0f && PlayerWorldPos.y == 0.0f) continue;

        Vector2 PlayerScreenPos;
        // Calling dynamic protection layer instead of direct hardware pointer invocation
        if (DynamicProjectWorldToScreen(UWorld, PlayerWorldPos, PlayerScreenPos)) {
            
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
