//
//  MyCustomAnimator.h
//  TestOSXApp
//
//  Created by jonathan on 22/01/2015.
//  Copyright (c) 2015 net.ellipsis. All rights reserved.
//

#import <Foundation/Foundation.h>
@import AppKit;

@interface PFCustomAnimator : NSObject <NSViewControllerPresentationAnimator>
@property (weak) NSViewController *source;
@end
