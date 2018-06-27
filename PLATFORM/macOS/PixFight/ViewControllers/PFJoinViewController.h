//
//  PFJoinViewController.h
//  PixFight
//
//  Created by Marcin Małysz on 27/06/2018.
//  Copyright © 2018 Marcin Małysz. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "PFMultiplayerClient.hpp"

@interface PFJoinViewController : NSViewController

@property (nonatomic, assign) std::shared_ptr<PFMultiplayerClient> client;

@end
