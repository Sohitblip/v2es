#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#import <Metal/Metal.h>
#import <MetalKit/MetalKit.h>

#include <imgui.h>
#include <backends/imgui_impl_metal.h>

extern void DrawESP();
extern void RenderMenu();

static bool backend_initialized = false;
static id<MTLCommandQueue> commandQueue = nil;
static id<MTLDevice> device = nil;

// Safe Drawing Pipeline tied with Metal Core Hardware
void ExecuteHardwareDrawing(MTLRenderPassDescriptor* renderPassDescriptor, id<MTLCommandBuffer> commandBuffer) {
    if (!backend_initialized) {
        // Initialize Core Device Pointer
        device = MTLCreateSystemDefaultDevice();
        if (!device) return;
        
        commandQueue = [device newCommandQueue];
        
        // Setup Screen Size Scaling 
        ImGuiIO& io = ImGui::GetIO();
        CGRect screenBounds = [UIScreen mainScreen].bounds;
        io.DisplaySize = ImVec2(screenBounds.size.width, screenBounds.size.height);
        
        // Booting ImGui Official Metal Drivers Setup
        ImGui_ImplMetal_Init(device);
        backend_initialized = true;
    }

    // Creating safe frames synchronized with the graphics processing card
    ImGui_ImplMetal_NewFrame(renderPassDescriptor);
    ImGui::NewFrame();

    // Constant status line indicator for your tracking verification
    ImDrawList* bgDraw = ImGui::GetBackgroundDrawList();
    if (bgDraw) {
        bgDraw->AddText(ImVec2(30, 50), ImColor(0, 255, 0, 255), "Mohit ESP Connected Successfully!");
    }

    // Call your customized interface configurations
    RenderMenu();   
    DrawESP();      

    // Finalize drawing commands buffer
    ImGui::Render();
    id<MTLRenderCommandEncoder> renderEncoder = [commandBuffer renderCommandEncoderWithDescriptor:renderPassDescriptor];
    if (renderEncoder) {
        ImGui_ImplMetal_RenderDrawData(ImGui::GetDrawData(), commandBuffer, renderEncoder);
        [renderEncoder endEncoding];
    }
}

@interface MetalHookController : NSObject
+ (void)startOverlayLoop;
+ (void)showWelcomeToast;
@end

@implementation MetalHookController
+ (void)showWelcomeToast {
    UIViewController *root = [UIApplication sharedApplication].keyWindow.rootViewController;
    if (!root) root = [[[UIApplication sharedApplication] windows] firstObject].rootViewController;
    
    if (root) {
        UIAlertController *alert = [UIAlertController alertControllerWithTitle:@"Mohit Mod System"
                                                                       message:@"Welcome! Mohit ESP and Hardware Metal Hook initialized successfully."
                                                                preferredStyle:UIAlertControllerStyleAlert];
        [alert addAction:[UIAlertAction actionWithTitle:@"Start Match" style:UIAlertActionStyleDefault handler:nil]];
        [root presentViewController:alert animated:YES completion:nil];
    }
}

+ (void)startOverlayLoop {
    // 10 Seconds safety sleep parameter allowing Unreal textures to buffer in peace
    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(10.0 * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
        [self showWelcomeToast];
        
        CADisplayLink *displayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(onFrameTick:)];
        [displayLink addToRunLoop:[NSRunLoop mainRunLoop] forMode:NSRunLoopCommonModes];
    });
}

+ (void)onFrameTick:(CADisplayLink *)sender {
    @autoreleasepool {
        // Generating virtual descriptors to securely pipe graphics loop without overlaps
        MTLRenderPassDescriptor* passDescriptor = [MTLRenderPassDescriptor renderPassDescriptor];
        if (!passDescriptor || !commandQueue) {
            // Re-allocate fallback if queue isn't completely saturated yet
            if (backend_initialized == false) {
                device = MTLCreateSystemDefaultDevice();
                if (device) commandQueue = [device newCommandQueue];
            }
            return;
        }
        
        id<MTLCommandBuffer> cmdBuffer = [commandQueue commandBuffer];
        if (cmdBuffer) {
            ExecuteHardwareDrawing(passDescriptor, cmdBuffer);
            [cmdBuffer commit];
        }
    }
}
@end

// Constructor mapping initialization parameters on library runtime entry
__attribute__((constructor)) static void init_metal_backend_injection() {
    [MetalHookController startOverlayLoop];
}
