//
//  PFRenderViewController.h
//  PixFight
//
//  Created by Marcin Małysz on 06/01/2018.
//  Copyright © 2018 Marcin Małysz. All rights reserved.
//

#import <GLKit/GLKit.h>
#import <GLKit/GLKViewController.h>

@interface PFRenderViewController : GLKViewController

- (void)startGame:(NSString *)mapName
         playerID:(NSInteger)playerID
      playersSize:(NSInteger)playersCount;

- (void)loadGamePath:(NSString *)filePath;

@end
