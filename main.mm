#include <Foundation/Foundation.h>
#include <imgui.h>
#include <UIKit/UIKit.h>

extern void HackRenderLoop(); // Fetch render hook function pointer

// Multi-finger gesture initialization configurations
static void InitializeOverlayUI() {
    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(5.0 * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
        // Auto initialize standard basic setups
        ImGuiIO& io = ImGui::GetIO();
        io.DisplaySize = ImVec2([UIScreen mainScreen].bounds.size.width, [UIScreen mainScreen].bounds.size.height);
        
        // Setup direct render thread intercept loop notifier
        // (Runs inside background thread engine clock)
        NSTimer *renderTimer = [NSTimer timerWithTimeInterval:1.0/60.0 target:[NSBlockOperation blockOperationWithBlock:^{
            HackRenderLoop();
        }] selector:@selector(main) userInfo:nil repeats:YES];
        [[NSRunLoop mainRunLoop] addTimer:renderTimer forMode:NSRunLoopCommonModes];
    });
}

// iOS native library automatic loader entry constructor point
__attribute__((constructor)) static void initialize_dylib_entry() {
    // This loads the hooks immediately when the game bundle initializes
    InitializeOverlayUI();
}
