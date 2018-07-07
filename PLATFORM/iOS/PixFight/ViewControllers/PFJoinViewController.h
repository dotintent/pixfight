//
//  PFJoinViewController.h
//  PixFight
//
//  Created by Marcin Małysz on 17/06/2018.
//  Copyright © 2018 Marcin Małysz. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "PFMultiplayerClient.hpp"

@interface PFJoinViewController : UIViewController

@property (nonatomic, assign) std::shared_ptr<PFMultiplayerClient> client;

@end
