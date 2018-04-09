//
//  PFNewGameViewController.m
//  PixFight
//
//  Created by Marcin Małysz on 24/01/2018.
//  Copyright © 2018 Marcin Małysz. All rights reserved.
//

#import "PFNewGameViewController.h"
#import "PFSelectTeamController.h"
#import "PFRenderViewController.h"

#import "NSView+DisableActions.h"

@interface PFNewGameViewController ()<NSTableViewDelegate, NSTableViewDataSource, PFSelectTeamControllerDelegate>

@property (weak) IBOutlet NSTableView *mapPoolTableView;
@property (weak) IBOutlet NSImageView *previewMapImage;
@property (weak) IBOutlet NSButton *selectTeamButton;
@property (weak) IBOutlet NSButton *startGameButton;
@property (nonatomic, strong) NSDictionary *data;
@property (nonatomic, assign) NSInteger selectedPlayer;
@property (nonatomic, assign) NSInteger playersPlaying;

@end

@implementation PFNewGameViewController

- (void)viewDidLoad {
    [super viewDidLoad];

    self.selectedPlayer = 1;
    self.playersPlaying = 2;

    self.mapPoolTableView.delegate = self;
    self.mapPoolTableView.dataSource = self;

    self.data = @{

          @(0) : @{
                  @"name" : @"TUTORIAL",
                  @"image" : @"tutorial.png",
                  @"mapname" : @"tutorial",
                  @"players" : @(2)
                  },
          @(1) : @{
                  @"name" : @"HUNT",
                  @"image" : @"Hunt.png",
                  @"mapname" : @"Hunt",
                  @"players" : @(3)
                  },
          @(2) : @{
                  @"name" : @"PASAGE",
                  @"image" : @"pasage.png",
                  @"mapname" : @"pasage",
                  @"players" : @(2)
                  },
          @(3) : @{
                  @"name" : @"WATERWAY",
                  @"image" : @"waterway.png",
                  @"mapname" : @"waterway",
                  @"players" : @(2)
                  },
          @(4) : @{
                  @"name" : @"KINGOFHILL",
                  @"image" : @"kingofhill.png",
                  @"mapname" : @"kingofhill",
                  @"players" : @(4)
                  },
          };

    self.mapPoolTableView.allowsColumnSelection = NO;
    [self.mapPoolTableView reloadData];

    [self.mapPoolTableView selectColumnIndexes:[NSIndexSet indexSetWithIndex:0]
                          byExtendingSelection:NO];
    [self.mapPoolTableView selectRowIndexes:[NSIndexSet indexSetWithIndex:0]
                       byExtendingSelection:NO];
}

- (void)viewWillAppear {
    [super viewWillAppear];

    if (self.mapPoolTableView.hidden) {

        [self dismissController:nil];
    }

    self.mapPoolTableView.hidden = NO;
    [self.view enableSubViews];
}

- (void)viewWillDisappear {
    [super viewWillDisappear];
    [self.view disableSubViews];
}

- (IBAction)backButtonAction:(NSButton *)sender {

    [sender setState:NSControlStateValueOff];
    [self dismissViewController:self];
}

- (IBAction)startGameAction:(NSButton *)sender {

    self.mapPoolTableView.hidden = YES;

    [sender setState:NSControlStateValueOff];
    [self performSegueWithIdentifier:@"PFRenderSegueIdentifier" sender:self];
}

- (IBAction)selectTeamAction:(NSButton *)sender {

    [sender setState:NSControlStateValueOff];
    [self performSegueWithIdentifier:@"PFSelecTeamSegueIdentifier" sender:self];
}

- (void)prepareForSegue:(NSStoryboardSegue *)segue sender:(id)sender {
    
    if ([segue.identifier isEqualToString:@"PFRenderSegueIdentifier"]) {
        
        PFRenderViewController *controller = segue.destinationController;
        
        NSInteger row = self.mapPoolTableView.selectedRow;
        NSDictionary *data = self.data[@(row)];
        
        NSString *mapName = data[@"mapname"];
        
        [controller startGame:mapName
                     playerID:self.selectedPlayer
                  playersSize:self.playersPlaying];

    }
    else if ([segue.identifier isEqualToString:@"PFSelecTeamSegueIdentifier"]) {
        
        PFSelectTeamController *controller = segue.destinationController;
        controller.delegate = self;
        [controller setNumberOfPlayers:self.playersPlaying];
    }
}

- (NSInteger)numberOfRowsInTableView:(NSTableView *)tableView {

    return self.data.count;
}

- (nullable id)tableView:(NSTableView *)tableView objectValueForTableColumn:(nullable NSTableColumn *)tableColumn row:(NSInteger)row {

    return self.data[@(row)][@"name"];
}

- (void)tableViewSelectionDidChange:(NSNotification *)notification {

    NSInteger row = self.mapPoolTableView.selectedRow;

    if (row < 0) {
        return;
    }

    NSDictionary *data = self.data[@(row)];

    self.previewMapImage.image = [NSImage imageNamed:data[@"image"]];
    self.playersPlaying = [self.data[@(row)][@"players"] integerValue];
    self.selectedPlayer = 1;

    [self.selectTeamButton setTitle:@"BLUE"];
}

- (void)selectTeamController:(PFSelectTeamController *)sender
               didSelectTeam:(NSInteger)team {
    
    self.selectedPlayer = team;
    
    switch (team) {
        case 1: [self.selectTeamButton setTitle:@"BLUE"];
            break;
        case 2: [self.selectTeamButton setTitle:@"RED"];
            break;
        case 3: [self.selectTeamButton setTitle:@"GREEN"];
            break;
        case 4: [self.selectTeamButton setTitle:@"YELLOW"];
            break;
        default:
            break;
    }
}

@end
