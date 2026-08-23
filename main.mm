#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#include <imgui.h>

extern void DrawESP();
extern void RenderMenu();

static bool backend_allocated = false;

// Hardware Safe Drawing Call
void FireOverlayPipeline() {
    if (!backend_allocated) {
        ImGuiIO& io = ImGui::GetIO();
        CGRect bounds = [UIScreen mainScreen].bounds;
        io.DisplaySize = ImVec2(bounds.size.width, bounds.size.height);
        backend_allocated = true;
    }

    ImGui::NewFrame();
    
    // Green color text watermark top-left corner par
    ImDrawList* backgroundDrawList = ImGui::GetBackgroundDrawList();
    if (backgroundDrawList) {
        backgroundDrawList->AddText(ImVec2(30, 50), ImColor(0, 255, 0, 255), "Mohit ESP Connected Successfully!");
    }

    RenderMenu();   
    DrawESP();      
    ImGui::Render();
}

@interface MetalHookHandler : NSObject
+ (void)startOverlayClock;
+ (void)showMohitWelcomeAlert;
@end

@implementation MetalHookHandler

// Game open hote hi iOS Welcome Alert popup screen par aayega
+ (void)showMohitWelcomeAlert {
    UIViewController *rootVC = [UIApplication sharedApplication].keyWindow.rootViewController;
    if (!rootVC) {
        rootVC = [[[UIApplication sharedApplication] windows] firstObject].rootViewController;
    }
    
    if (rootVC) {
        UIAlertController *alert = [UIAlertController alertControllerWithTitle:@"Mohit Mod System"
                                                                       message:@"Welcome! Mohit ESP is now injected and working fine."
                                                                preferredStyle:UIAlertControllerStyleAlert];
        
        UIAlertAction *okAction = [UIAlertAction actionWithTitle:@"Start Match"
                                                           style:UIAlertActionStyleDefault
                                                         handler:nil];
        [alert addAction:okAction];
        [rootVC presentViewController:alert animated:YES completion:nil];
    }
}

+ (void)startOverlayClock {
    // 10 Seconds heavy buffer delay taaki Unreal Engine data stable ho sake
    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(10.0 * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
        
        [self showMohitWelcomeAlert];
        
        CADisplayLink *displayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(renderTick:)];
        [displayLink addToRunLoop:[NSRunLoop mainRunLoop] forMode:NSRunLoopCommonModes];
    });
}

+ (void)renderTick:(CADisplayLink *)sender {
    @autoreleasepool {
        FireOverlayPipeline();
    }
}
@end

__attribute__((constructor)) static void load_safe_esp_entry() {
    [MetalHookHandler startOverlayClock];
}
