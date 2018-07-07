//
//  PFSelectMapController.h
//  PixFight
//
//  Created by Marcin Małysz on 26/06/2018.
//  Copyright © 2018 Marcin Małysz. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@class PFSelectMapController;

@protocol PFSelectMapControllerDelegate
- (void)selectMapController:(PFSelectMapController *)sender didSelectMap:(NSNumber *)mapNumber;
@end

@interface PFSelectMapController : NSViewController
@property (nonatomic, weak) id<PFSelectMapControllerDelegate> delegate;
@end
