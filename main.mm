#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#include <imgui.h>

extern void DrawESP(); // Fetching core geometry graphics loop
extern void RenderMenu(); // Fetching menu design handler

static bool is_imgui_ready = false;

// Hardware window hook intercepting graphics clock safely
void SafeRenderPipeline() {
    if (!is_imgui_ready) {
        ImGuiIO& io = ImGui::GetIO();
        io.DisplaySize = ImVec2([UIScreen mainScreen].bounds.size.width, [UIScreen mainScreen].bounds.size.height);
        is_imgui_ready = true;
    }

    ImGui::NewFrame();
    RenderMenu();   // Draw Layout Box UI
    DrawESP();      // Draw Line ESP geometry lines
    ImGui::Render();
}

// System event validation filter layout
@interface OverlayHookController : NSObject
+ (void)loadHookLoop;
@end

@implementation OverlayHookController
+ (void)loadHookLoop {
    // 8 seconds safety pause for Unreal Engine assets loading safely into RAM
    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(8.0 * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
        CADisplayLink *displayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(onFrameUpdate:)];
        [displayLink addToRunLoop:[NSRunLoop mainRunLoop] forMode:NSDefaultRunLoopMode];
    });
}

+ (void)onFrameUpdate:(CADisplayLink *)sender {
    @autoreleasepool {
        SafeRenderPipeline();
    }
}
@end

// Core construction lifecycle initializer mapping target architecture context
__attribute__((constructor)) static void init_ios_esp_hook() {
    [OverlayHookController loadHookLoop];
}
