//
//  ViewController.m
//  PixFight
//
//  Created by Marcin Małysz on 23/01/2018.
//  Copyright © 2018 Marcin Małysz. All rights reserved.
//

#import "PFMenuViewController.h"
#import "NSView+DisableActions.h"

@implementation PFMenuViewController

- (void)viewDidLoad {
    [super viewDidLoad];

    [self setTitle:@"PixFight"];
}

- (void)viewWillAppear {
    [super viewWillAppear];
    [self.view enableSubViews];
}

- (void)viewWillDisappear {
    [super viewWillDisappear];
    [self.view disableSubViews];
}

- (IBAction)skirmishAction:(NSButton *)sender {

    [sender setState:NSControlStateValueOff];
    [self performSegueWithIdentifier:@"PFNewGameSegueIdentifier" sender:self];
}

- (IBAction)loadGameAction:(NSButton *)sender {

    [sender setState:NSControlStateValueOff];
    [self performSegueWithIdentifier:@"PFLoadSegueIdentifier" sender:self];
}

- (IBAction)settingsAction:(NSButton *)sender {

    [sender setState:NSControlStateValueOff];
    [self performSegueWithIdentifier:@"PFSettingsSegueIdentifier" sender:self];
}

@end
