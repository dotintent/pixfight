//
//  MyCustomSegue.m
//  NSViewControllerPresentations
//
//  Created by jonathan on 25/01/2015.
//  Copyright (c) 2015 net.ellipsis. All rights reserved.
//

#import "PFCustomSegue.h"
#import "PFCustomAnimator.h"

@implementation PFCustomSegue

- (void)perform {

    PFCustomAnimator *animator = [[PFCustomAnimator alloc] init];
    animator.source = self.sourceController;
    [self.sourceController presentViewController:self.destinationController
                                        animator:animator];
}

@end
