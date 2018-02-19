//
//  PFRenderViewController.h
//  PixFight
//
//  Created by Marcin Małysz on 25/01/2018.
//  Copyright © 2018 Marcin Małysz. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface PFRenderViewController : NSViewController

- (void)startGame:(NSString *)mapName
         playerID:(NSInteger)playerID
      playersSize:(NSInteger)playersCount;

- (void)loadGamePath:(NSString *)filePath;

@end
