//
//  PFRoomViewController.h
//  PixFight
//
//  Created by Marcin Małysz on 26/06/2018.
//  Copyright © 2018 Marcin Małysz. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "PFMultiplayerClient.hpp"

@interface PFRoomViewController : NSViewController
@property (nonatomic, assign) std::shared_ptr<PFMultiplayerClient> client;
@property (nonatomic, assign, getter=isMaster) BOOL master;
@end
