//
//  AppDelegate+Audio.h
//  PixFight
//
//  Created by Marcin Małysz on 24/01/2018.
//  Copyright © 2018 Marcin Małysz. All rights reserved.
//

#import "AppDelegate.h"
#import "Audio.hpp"

@interface AppDelegate (Audio)

- (Audio *)audioUnit;
- (void)playSelectSound;
- (void)loadMusicNamed:(NSString *)name;

@end
