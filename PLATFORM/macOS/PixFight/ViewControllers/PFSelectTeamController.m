//
//  PFSelectTeamController.m
//  PixFight
//
//  Created by Marcin Małysz on 25/01/2018.
//  Copyright © 2018 Marcin Małysz. All rights reserved.
//

#import "PFSelectTeamController.h"

@interface PFSelectTeamController () {

    NSInteger _numberOfPlayers;
}

@property (weak) IBOutlet NSButton *blueButton;
@property (weak) IBOutlet NSButton *redButton;
@property (weak) IBOutlet NSButton *greenButton;
@property (weak) IBOutlet NSButton *yellowButton;

@end

@implementation PFSelectTeamController

- (void)viewWillAppear {
    [super viewWillAppear];

    switch (_numberOfPlayers) {
        case 2: {
        
            self.greenButton.hidden = YES;
            self.yellowButton.hidden = YES;
            
            self.preferredContentSize = NSMakeSize(200, 102);
        }
            break;
            
        case 3: {
            
            self.yellowButton.hidden = YES;
            
            self.preferredContentSize = NSMakeSize(200, 139);
        }
            break;
        default: {
            
            self.preferredContentSize = NSMakeSize(200, 172);
        }
            break;
    }

}

- (void)setNumberOfPlayers:(NSInteger)numerOfPlayers {

    _numberOfPlayers = numerOfPlayers;
}

- (IBAction)blueAction:(id)sender {

    [self.delegate selectTeamController:self didSelectTeam:1];
    [self dismissController:self];
}

- (IBAction)redAction:(id)sender {

    [self.delegate selectTeamController:self didSelectTeam:2];
    [self dismissController:self];
}

- (IBAction)greenAction:(id)sender {

    [self.delegate selectTeamController:self didSelectTeam:3];
    [self dismissController:self];
}

- (IBAction)yellowAction:(id)sender {

    [self.delegate selectTeamController:self didSelectTeam:4];
    [self dismissController:self];
}

@end
