//
//  PFSelectTeamController.h
//  PixFight
//
//  Created by Marcin Małysz on 25/01/2018.
//  Copyright © 2018 Marcin Małysz. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@class PFSelectTeamController;

@protocol PFSelectTeamControllerDelegate <NSObject>

- (void)selectTeamController:(PFSelectTeamController *)sender
               didSelectTeam:(NSInteger)team;

@end

@interface PFSelectTeamController : NSViewController

@property (weak) id<PFSelectTeamControllerDelegate> delegate;

- (void)setNumberOfPlayers:(NSInteger)numerOfPlayers;

@end

