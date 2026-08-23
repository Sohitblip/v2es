#include <imgui.h>
#include <vector>
#include <mach/mach.h>
#include "Offsets.hpp"

struct Vector3 { float x, y, z; };
struct Vector2 { float x, y; };

// Kernel level Safe Memory Reader - 100% Stops Crash
template <typename T>
T SafeRead(uintptr_t address) {
    T buffer;
    vm_size_t size = sizeof(T);
    vm_size_t bytesRead = 0;
    
    // Agar memory address valid nahi hai toh default empty data return karega, crash nahi karega
    kern_return_t kr = vm_read_overwrite(mach_task_self(), (vm_address_t)address, size, (vm_address_t)&buffer, &bytesRead);
    if (kr != KERN_SUCCESS) {
        return T(); 
    }
    return buffer;
}

bool ProjectWorldLocationToScreen(uintptr_t basic_pointer, Vector3 WorldLocation, Vector2& ScreenLocation) {
    if (!basic_pointer) return false;
    typedef bool (*_ProjectWorldToScreen)(uintptr_t, Vector3, Vector2&);
    
    auto function_ptr = (_ProjectWorldToScreen)(OFFSET_PROJECT_WORLD_TO_SCREEN);
    if (!function_ptr) return false;
    
    return function_ptr(basic_pointer, WorldLocation, ScreenLocation);
}

void DrawESP() {
    if (!Settings::EnableESP) return;

    ImDrawList* drawList = ImGui::GetBackgroundDrawList();
    if (!drawList) return; 

    ImVec2 screenSize = ImGui::GetIO().DisplaySize;
    ImVec2 screenCenter = ImVec2(screenSize.x / 2.0f, screenSize.y / 2.0f);

    // Safe reading UWorld Pointer
    uintptr_t UWorld = SafeRead<uintptr_t>(OFFSET_UWORLD);
    if (!UWorld || UWorld < 0x100000000) return; 

    // Safe reading ActorArray Pointer
    uintptr_t ActorArray = SafeRead<uintptr_t>(UWorld + OFFSET_ACTOR_ARRAY);
    if (!ActorArray || ActorArray < 0x100000000) return; 

    // Safe reading ActorCount Boundaries
    int ActorCount = SafeRead<int>(UWorld + OFFSET_ACTOR_ARRAY + 0x8);
    if (ActorCount <= 0 || ActorCount > 300) return; 

    for (int i = 0; i < ActorCount; i++) {
        uintptr_t CurrentActor = SafeRead<uintptr_t>(ActorArray + (i * 0x8));
        if (!CurrentActor || CurrentActor < 0x100000000) continue;

        uintptr_t RootComponent = SafeRead<uintptr_t>(CurrentActor + 0x140); 
        if (!RootComponent || RootComponent < 0x100000000) continue;
        
        Vector3 PlayerWorldPos = SafeRead<Vector3>(RootComponent + 0x1D0);
        if (PlayerWorldPos.x == 0.0f && PlayerWorldPos.y == 0.0f) continue;

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
