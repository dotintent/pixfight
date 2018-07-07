//
//  PFSelectMapController.m
//  PixFight
//
//  Created by Marcin Małysz on 26/06/2018.
//  Copyright © 2018 Marcin Małysz. All rights reserved.
//

#import "PFSelectMapController.h"

@interface PFSelectMapController ()

@end

@implementation PFSelectMapController

- (void)viewDidLoad {
    [super viewDidLoad];
    self.preferredContentSize = NSMakeSize(242, 192);
}

- (IBAction)tutorialAction:(id)sender {

    [self.delegate selectMapController:self didSelectMap:@(0)];
}

- (IBAction)huntlAction:(id)sender {

    [self.delegate selectMapController:self didSelectMap:@(1)];
}

- (IBAction)pasageAction:(id)sender {

    [self.delegate selectMapController:self didSelectMap:@(2)];
}

- (IBAction)waterwayAction:(id)sender {

    [self.delegate selectMapController:self didSelectMap:@(3)];
}

- (IBAction)kingofhillAction:(id)sender {

    [self.delegate selectMapController:self didSelectMap:@(4)];
}

@end
