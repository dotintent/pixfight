//
//  MyCustomAnimator.m
//  TestOSXApp
//
//  Created by jonathan on 22/01/2015.
//  Copyright (c) 2015 net.ellipsis. All rights reserved.

#import "PFCustomAnimator.h"

@implementation PFCustomAnimator

- (void)animatePresentationOfViewController:(NSViewController *)viewController
                         fromViewController:(NSViewController *)fromViewController {
    
    NSViewController* bottomVC = fromViewController;
    NSViewController* topVC = viewController;
    
    topVC.view.wantsLayer = YES;
    topVC.view.layerContentsRedrawPolicy = NSViewLayerContentsRedrawOnSetNeedsDisplay;
    topVC.view.alphaValue = 0;
    
    [bottomVC.view addSubview:topVC.view];

    CGRect frame = NSRectToCGRect(bottomVC.view.frame);

    [topVC.view setFrame:NSRectFromCGRect(frame)];
    topVC.view.layer.backgroundColor = [NSColor blackColor].CGColor;
    
    [bottomVC viewWillDisappear];
    
    [NSAnimationContext runAnimationGroup:^(NSAnimationContext *context) {
       
        context.duration = 0.35;
        topVC.view.animator.alphaValue = 1.0;
        
    } completionHandler:nil];
    
}

- (void)animateDismissalOfViewController:(NSViewController *)viewController
                      fromViewController:(NSViewController *)fromViewController {
    
    NSViewController* bottomVC = self.source;
    NSViewController* topVC = viewController;
    
    topVC.view.wantsLayer = YES;
    topVC.view.layerContentsRedrawPolicy = NSViewLayerContentsRedrawOnSetNeedsDisplay;

    [topVC viewWillDisappear];
    [bottomVC viewWillAppear];
    
    [NSAnimationContext runAnimationGroup:^(NSAnimationContext *context) {
        
        context.duration = 0.35;
        topVC.view.animator.alphaValue = 0;
        
    } completionHandler:^{
        
        [topVC.view removeFromSuperview];
    }];
}

@end
