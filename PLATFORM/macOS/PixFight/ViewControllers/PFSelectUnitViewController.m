//
//  FPSelectUnitViewController.m
//  PixFight
//
//  Created by Marcin Małysz on 26/01/2018.
//  Copyright © 2018 Marcin Małysz. All rights reserved.
//

#import "PFSelectUnitViewController.h"

@interface PFSelectUnitViewController ()

@property (weak) IBOutlet NSTextField *cashLabel;
@property (weak) IBOutlet NSButton *infantryButton;
@property (weak) IBOutlet NSButton *bazookaButton;
@property (weak) IBOutlet NSButton *jeepButton;
@property (weak) IBOutlet NSButton *tankButton;
@property (weak) IBOutlet NSButton *artilleryButton;

@property (strong) NSDictionary *data;
@property (assign) int currentMoney;
@property (assign) int teamID;

@end

@implementation PFSelectUnitViewController

- (void)viewDidLoad {
    [super viewDidLoad];

    self.data = @{
                  @(0) : @{
                          @"image" : @"icon_infantry",
                          @"name" : @"Infantry",
                          @"cost" : @(75)
                          },

                  @(1) : @{
                          @"image" : @"icon_bazooka",
                          @"name" : @"Bazooka",
                          @"cost" : @(150)
                          },

                  @(2) : @{
                          @"image" : @"icon_jeep",
                          @"name" : @"Jeep",
                          @"cost" : @(200)
                          },

                  @(3) : @{
                          @"image" : @"icon_tank",
                          @"name" : @"Tank",
                          @"cost" : @(300)
                          },

                  @(4) : @{
                          @"image" : @"icon_artillery",
                          @"name" : @"Artillery",
                          @"cost" : @(200)
                          },
                  };

    self.preferredContentSize = NSMakeSize(450, 400);
}

- (void)viewWillAppear {
    [super viewWillAppear];

    self.cashLabel.stringValue = [NSString stringWithFormat:@"Cash: %@", @(self.currentMoney)];

    NSArray *buttons = @[
                         self.infantryButton,
                         self.bazookaButton,
                         self.jeepButton,
                         self.tankButton,
                         self.artilleryButton
                         ];

    for (int i = 0; i < buttons.count; ++i) {

        NSButton *button = buttons[i];

        NSDictionary *data = self.data[@(i)];
        NSInteger cost = [data[@"cost"] integerValue];

        NSString *title = [NSString stringWithFormat:@"%@ - %@", data[@"name"], @(cost)];
        NSMutableAttributedString *attributed = [[NSMutableAttributedString alloc] initWithString:title];

        BOOL expensive = cost > self.currentMoney;
        NSColor *color = expensive ? [NSColor redColor] : [NSColor blackColor];

        NSRange titleRange = NSMakeRange(0, [attributed length]);
        [attributed addAttribute:NSForegroundColorAttributeName value:color range:titleRange];

        [button setAttributedTitle:attributed];
        [button setImage:[NSImage imageNamed:[NSString stringWithFormat:@"%@%@.png", data[@"image"], @(self.teamID)]]];

        button.enabled = !expensive;
    }
}

- (IBAction)cancelAction:(id)sender {

    [self dismissController:nil];
}

- (void)startWithTeamID:(int)teamID
            playerMoney:(int)availableMoney {

    self.teamID = teamID;
    self.currentMoney = availableMoney;
}

- (int)remainedCash {

    return self.currentMoney;
}

- (IBAction)infantryAction:(NSButton *)sender {

    NSDictionary *cellData = self.data[@(0)];

    self.currentMoney -= (int)[cellData[@"cost"] integerValue];
    [self.delegate selectUnitView:self didFinishWithUnitID:0];

    [self dismissController:nil];
}

- (IBAction)bazookaAction:(NSButton *)sender {

    NSDictionary *cellData = self.data[@(1)];

    self.currentMoney -= (int)[cellData[@"cost"] integerValue];
    [self.delegate selectUnitView:self didFinishWithUnitID:1];

    [self dismissController:nil];
}

- (IBAction)jeepAction:(NSButton *)sender {

    NSDictionary *cellData = self.data[@(2)];

    self.currentMoney -= (int)[cellData[@"cost"] integerValue];
    [self.delegate selectUnitView:self didFinishWithUnitID:2];

    [self dismissController:nil];
}

- (IBAction)tankAction:(NSButton *)sender {

    NSDictionary *cellData = self.data[@(3)];

    self.currentMoney -= (int)[cellData[@"cost"] integerValue];
    [self.delegate selectUnitView:self didFinishWithUnitID:3];

    [self dismissController:nil];
}

- (IBAction)artilleryAction:(NSButton *)sender {

    NSDictionary *cellData = self.data[@(4)];

    self.currentMoney -= (int)[cellData[@"cost"] integerValue];
    [self.delegate selectUnitView:self didFinishWithUnitID:4];

    [self dismissController:nil];
}

@end
