//
//  NSView+DisableActions.h
//  PixFight
//
//  Created by Marcin Małysz on 25/01/2018.
//  Copyright © 2018 Marcin Małysz. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface NSView (DisableActions)

- (void)disableSubViews;
- (void)enableSubViews;

@end
