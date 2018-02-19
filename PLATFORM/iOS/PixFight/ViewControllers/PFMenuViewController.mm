//
//  ViewController.m
//  PixFight
//
//  Created by Marcin Małysz on 01/01/2018.
//  Copyright © 2018 Marcin Małysz. All rights reserved.
//

#import "PFMenuViewController.h"
#import "AppDelegate+Audio.h"

@interface PFMenuViewController ()

@property (nonatomic, weak) AppDelegate *delegate;
@property (nonatomic, weak) IBOutlet NSLayoutConstraint *bottomButtonLayoutConstraint;
@property (nonatomic, weak) IBOutlet NSLayoutConstraint *topLogoLayoutConstraint;
@property (nonatomic, weak) IBOutlet NSLayoutConstraint *backgroundEqualWidthLayoutConstraint;

@end

@implementation PFMenuViewController

- (void)viewDidLoad {

    [super viewDidLoad];
    
    self.delegate = (AppDelegate *)[[UIApplication sharedApplication] delegate];
    
    if ([UIDevice currentDevice].userInterfaceIdiom == UIUserInterfaceIdiomPad) {
        
        self.bottomButtonLayoutConstraint.constant = 100;
        self.topLogoLayoutConstraint.constant = 200;
        
        [NSLayoutConstraint deactivateConstraints:@[self.backgroundEqualWidthLayoutConstraint]];
        [self.view layoutIfNeeded];
    }
}

- (IBAction)skirmisAction:(id)sender {

    [self.delegate playSelectSound];
    [self performSegueWithIdentifier:@"PFNewGameSegueIdentifier" sender:self];
}

- (IBAction)loadGameAction:(id)sender {

    [self.delegate playSelectSound];
    [self performSegueWithIdentifier:@"PFLoadGameSegueIdentifier" sender:self];
}

- (IBAction)settingsAction:(id)sender {

    [self.delegate playSelectSound];
    [self performSegueWithIdentifier:@"PFOptionSegueIdentifier" sender:self];
}

@end
