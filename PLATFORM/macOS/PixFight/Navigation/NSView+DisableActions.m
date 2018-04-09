//
//  NSView+DisableActions.m
//  PixFight
//
//  Created by Marcin Małysz on 25/01/2018.
//  Copyright © 2018 Marcin Małysz. All rights reserved.
//

#import "NSView+DisableActions.h"

@implementation NSView (DisableActions)

- (void)disableSubViews {
    
    [self setSubViewsEnabled:NO];
}

- (void)enableSubViews {
    
    [self setSubViewsEnabled:YES];
}

- (void)setSubViewsEnabled:(BOOL)enabled {
    
    NSView* currentView = nil;
    NSEnumerator* viewEnumerator = [[self subviews] objectEnumerator];
    
    while (currentView = [viewEnumerator nextObject]) {
        
        if ([currentView respondsToSelector:@selector(setEnabled:)]) {
            
            [(NSControl*)currentView setEnabled:enabled];
        }
    }
}


@end

