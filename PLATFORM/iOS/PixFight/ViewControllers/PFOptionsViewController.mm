//
//  PFOptionsViewController.m
//  PixFight
//
//  Created by Marcin Małysz on 06/01/2018.
//  Copyright © 2018 Marcin Małysz. All rights reserved.
//

#import "PFOptionsViewController.h"
#import "AppDelegate+Audio.h"

@interface PFOptionsViewController ()

@property (nonatomic, weak) IBOutlet NSLayoutConstraint *backgroundEqualWidthLayoutConstraint;
@property (nonatomic, weak) IBOutlet UISwitch *muteSwitch;
@property (nonatomic, weak) IBOutlet UISwitch *aiSwitch;
@property (nonatomic, weak) AppDelegate *delegate;

@end

@implementation PFOptionsViewController

- (void)viewDidLoad {
    [super viewDidLoad];

    self.delegate = (AppDelegate *)[UIApplication sharedApplication].delegate;
    
    if ([UIDevice currentDevice].userInterfaceIdiom == UIUserInterfaceIdiomPad) {

        [NSLayoutConstraint deactivateConstraints:@[self.backgroundEqualWidthLayoutConstraint]];
        [self.view layoutIfNeeded];
    }
}

- (void)viewWillAppear:(BOOL)animated {
    
    [super viewWillAppear:animated];
    
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    
    self.muteSwitch.on = [defaults boolForKey:@"mute"];
    self.aiSwitch.on = [defaults boolForKey:@"hardai"];
}

- (IBAction)dismissButtonAction:(id)sender {

    [self dismissViewControllerAnimated:YES completion:nil];
}

- (IBAction)muteChangeAction:(UISwitch *)sender {

    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    
    [defaults setBool:sender.isOn forKey:@"mute"];
    [defaults synchronize];
    
    Audio *audio = [self.delegate audioUnit];
    
    if (sender.isOn) {
        
        audio->mute();
    }
    else {
        
        audio->unmute();
    }
}

- (IBAction)aiChangeAction:(UISwitch *)sender {

    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    
    [defaults setBool:sender.isOn forKey:@"hardai"];
    [defaults synchronize];
}

@end
