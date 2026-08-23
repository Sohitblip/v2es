#pragma once

// Game Offsets (Sirf player location ke liye)
#define OFFSET_UWORLD                      0x10C034388
#define OFFSET_ACTOR_ARRAY                 0x1063693F0
#define OFFSET_PROJECT_WORLD_TO_SCREEN     0x1062B69B8

// Menu Toggles (Extern declares variables for other files to see)
namespace Settings {
    extern bool EnableESP;
    extern bool DrawLines;
    extern bool DrawBoxes;
}
