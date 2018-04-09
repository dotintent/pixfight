//
//  PFRendererGLView.m
//  PixFight
//
//  Created by Marcin Małysz on 25/01/2018.
//  Copyright © 2018 Marcin Małysz. All rights reserved.
//

#import "PFRendererGLView.h"

@interface PFRendererGLView () {

    CVDisplayLinkRef displayLink;
    GameLogic *_gameLogic;
}

@end

@implementation PFRendererGLView

//this function will run on another thread bind to OpenGL context so dispatch from and to this code.
- (void)drawView {
    
    [[self openGLContext] makeCurrentContext];
    
    CGLLockContext((CGLContextObj)[[self openGLContext] CGLContextObj]);
    
    if (_gameLogic) {
        
        _gameLogic->Render();
    }
    
    CGLFlushDrawable((CGLContextObj)[[self openGLContext] CGLContextObj]);
    CGLUnlockContext((CGLContextObj)[[self openGLContext] CGLContextObj]);

    dispatch_async(dispatch_get_main_queue(), ^{

        if (self.endRoundButton) {

            if (!_gameLogic) {
                return;
            }

            self.endRoundButton.enabled = _gameLogic->canEndTurn();
        }
    });
}

- (CVReturn)getFrameForTime:(const CVTimeStamp *)outputTime {
 
    @autoreleasepool {
        
        [self drawView];
    }
   
    return kCVReturnSuccess;
}

static CVReturn myDisplayLinkCallback(CVDisplayLinkRef displayLink,
                                      const CVTimeStamp* now,
                                      const CVTimeStamp* outputTime,
                                      CVOptionFlags flagsIn,
                                      CVOptionFlags* flagsOut,
                                      void* displayLinkContext) {
    
    return [(__bridge PFRendererGLView*)displayLinkContext getFrameForTime:outputTime];;
}

- (id)initWithFrame:(NSRect)frameRect {
    
    NSOpenGLPixelFormatAttribute attrs[] =
    {
        NSOpenGLPFADoubleBuffer,
        NSOpenGLPFADepthSize, 24,
        NSOpenGLPFAOpenGLProfile,
        NSOpenGLProfileVersion3_2Core,
        0
    };
    
    NSOpenGLPixelFormat *pf = [[NSOpenGLPixelFormat alloc] initWithAttributes:attrs];
    
    if (!pf) {
        
        NSLog(@"No OpenGL pixel format");
    }
    
    self = [super initWithFrame:frameRect pixelFormat:pf];
    
    if (self) {
        
        [self setAcceptsTouchEvents:YES];
    }
    
    return self;
}

- (void)startAnimating {

    CVDisplayLinkStart(displayLink);
}

- (void)stopAnimating {

    CVDisplayLinkStop(displayLink);
}

- (void)prepareOpenGL {
    
    [super prepareOpenGL];
    
    GLint swapInt = 0;
    [[self openGLContext] setValues:&swapInt forParameter:NSOpenGLCPSwapInterval];

    // Create a display link capable of being used with all active displays
    CVDisplayLinkCreateWithActiveCGDisplays(&displayLink);
    
    // Set the renderer output callback function
    CVDisplayLinkSetOutputCallback(displayLink, &myDisplayLinkCallback, (__bridge void*)self);
    
    // Set the display link for the current renderer
    CGLContextObj cglContext = (CGLContextObj)[[self openGLContext] CGLContextObj];
    CGLPixelFormatObj cglPixelFormat = (CGLPixelFormatObj)[[self pixelFormat] CGLPixelFormatObj];
    CVDisplayLinkSetCurrentCGDisplayFromOpenGLContext(displayLink, cglContext, cglPixelFormat);
    
    // Activate the display link
    [self startAnimating];
}

- (void)dealloc {
    
    // Release the display link
    [self stopAnimating];

    CVDisplayLinkRelease(displayLink);
}

- (void)setGameLogic:(GameLogic *)gameLogic {
    
    _gameLogic = gameLogic;
}

@end
