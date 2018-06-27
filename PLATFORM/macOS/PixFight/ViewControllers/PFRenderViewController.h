//
//  PFRenderViewController.h
//  PixFight
//
//  Created by Marcin Małysz on 25/01/2018.
//  Copyright © 2018 Marcin Małysz. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "PFMultiplayerClient.hpp"

@interface PFRenderViewController : NSViewController

@property(nonatomic, assign) std::shared_ptr<PFMultiplayerClient> client;

- (void)startGame:(NSString *)mapName
         playerID:(NSInteger)playerID
      playersSize:(NSInteger)playersCount;

- (void)loadGamePath:(NSString *)filePath;

@end
