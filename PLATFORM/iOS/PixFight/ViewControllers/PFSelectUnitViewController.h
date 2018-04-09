//
//  PFSelectUnitViewController.h
//  PixFight
//
//  Created by Marcin Małysz on 22/01/2018.
//  Copyright © 2018 Marcin Małysz. All rights reserved.
//

#import <UIKit/UIKit.h>

@class PFSelectUnitViewController;

@protocol PFSelectUnitViewControllerDelegate <NSObject>

- (void)selectUnitView:(PFSelectUnitViewController *)sender
   didFinishWithUnitID:(int)unitId;

@end

@interface PFSelectUnitViewController : UIViewController

@property (nonatomic, weak) id<PFSelectUnitViewControllerDelegate> delegate;

- (void)startWithTeamID:(int)teamID
            playerMoney:(int)availableMoney;

- (int)remainedCash;

@end
