//
//  PFGameMenuViewController.m
//  PixFight
//
//  Created by Marcin Małysz on 27/01/2018.
//  Copyright © 2018 Marcin Małysz. All rights reserved.
//

#import "PFGameMenuViewController.h"

@implementation PFGameMenuViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    self.preferredContentSize = NSMakeSize(260, 150);
}

- (IBAction)quitAction:(id)sender {

    [self dismissController:nil];
    [self.delegate gameMenuViewControllerDidQuit:self];
}

- (IBAction)saveAction:(id)sender {

    [self dismissController:nil];
    [self.delegate gameMenuViewControllerDidSave:self];
}

- (IBAction)cancelAction:(id)sender {

    [self dismissController:nil];
}

@end
