//
//  PFGameMenuViewController.h
//  PixFight
//
//  Created by Marcin Małysz on 27/01/2018.
//  Copyright © 2018 Marcin Małysz. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@class PFGameMenuViewController;

@protocol PFGameMenuViewControllerDelegate <NSObject>
- (void)gameMenuViewControllerDidQuit:(PFGameMenuViewController *)sender;
- (void)gameMenuViewControllerDidSave:(PFGameMenuViewController *)sender;
@end

@interface PFGameMenuViewController : NSViewController
@property (weak) id<PFGameMenuViewControllerDelegate> delegate;
@end
