//
//  PFRenderViewController.h
//  PixFight
//
//  Created by Marcin Małysz on 06/01/2018.
//  Copyright © 2018 Marcin Małysz. All rights reserved.
//

#import <GLKit/GLKit.h>
#import <GLKit/GLKViewController.h>

#import "PFMultiplayerClient.hpp"

@interface PFRenderViewController : GLKViewController

@property (nonatomic, assign) std::shared_ptr<PFMultiplayerClient> client;

- (void)startGame:(NSString *)mapName
         playerID:(NSInteger)playerID
      playersSize:(NSInteger)playersCount;

- (void)loadGamePath:(NSString *)filePath;

@end
