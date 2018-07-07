//
//  PFNewGameViewController.m
//  PixFight
//
//  Created by Marcin Małysz on 06/01/2018.
//  Copyright © 2018 Marcin Małysz. All rights reserved.
//

#import "PFNewGameViewController.h"
#import "PFRenderViewController.h"

@interface PFNewGameViewController ()<UITableViewDelegate, UITableViewDataSource, UIPopoverPresentationControllerDelegate>

@property (nonatomic, weak) IBOutlet NSLayoutConstraint *backgroundEqualWidthLayoutConstraint;
@property (nonatomic, weak) IBOutlet NSLayoutConstraint *selectTeamTopLayoutConstraints;
@property (weak, nonatomic) IBOutlet NSLayoutConstraint *startGameBottomLayoutConstraint;

@property (nonatomic, weak) IBOutlet UIImageView *previewMapImage;
@property (nonatomic, weak) IBOutlet UITableView *mapPoolTableView;
@property (nonatomic, weak) IBOutlet UIButton *selectTeamButton;
@property (nonatomic, weak) IBOutlet UIButton *startGameButton;
@property (nonatomic, strong) NSDictionary *data;
@property (nonatomic, assign) NSInteger selectedPlayer;
@property (nonatomic, assign) NSInteger playersPlaying;

@end

@implementation PFNewGameViewController

- (void)viewDidLoad {
    [super viewDidLoad];

    self.selectedPlayer = 1;
    self.playersPlaying = 2;
    
    if ([UIDevice currentDevice].userInterfaceIdiom == UIUserInterfaceIdiomPad) {

        [NSLayoutConstraint deactivateConstraints:@[self.backgroundEqualWidthLayoutConstraint]];

        self.selectTeamTopLayoutConstraints.constant = 100;
        self.startGameBottomLayoutConstraint.constant = 100;

        [self.view layoutIfNeeded];
    }

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

    [self.mapPoolTableView reloadData];
    [self.mapPoolTableView selectRowAtIndexPath:[NSIndexPath indexPathForRow:0 inSection:0]
                                       animated:NO
                                 scrollPosition:UITableViewScrollPositionNone];
}

- (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender {

    PFRenderViewController *gameController = segue.destinationViewController;

    NSIndexPath *selectedPath = self.mapPoolTableView.indexPathForSelectedRow;
    NSDictionary *data = self.data[@(selectedPath.row)];

    NSInteger players = [data[@"players"] integerValue];
    NSString *mapName = data[@"mapname"];

    [gameController startGame:mapName
                     playerID:self.selectedPlayer
                  playersSize:players];
}

- (IBAction)startGameAction:(id)sender {

    [self performSegueWithIdentifier:@"PFStartSegueIdentifier" sender:self];
}

- (IBAction)selectTeamAction:(id)sender {

    UIAlertController *selectTeamActionViewController = [UIAlertController alertControllerWithTitle:@"Choose team"
                                                                                            message:nil
                                                                                     preferredStyle:UIAlertControllerStyleActionSheet];

    UIAlertAction *cancelAction = [UIAlertAction actionWithTitle:@"Cancel"
                                                           style:UIAlertActionStyleCancel
                                                         handler:nil];

    UIAlertAction *blueTeam = [UIAlertAction actionWithTitle:@"BLUE"
                                                       style:UIAlertActionStyleDefault
                                                     handler:^(UIAlertAction *action){

                                                         self.selectedPlayer = 1;
                                                         [self.selectTeamButton setTitle:@"BLUE" forState:UIControlStateNormal];
                                                     }];

    UIAlertAction *redTeam = [UIAlertAction actionWithTitle:@"RED"
                                                      style:UIAlertActionStyleDefault
                                                    handler:^(UIAlertAction *action){

                                                        self.selectedPlayer = 2;
                                                        [self.selectTeamButton setTitle:@"RED" forState:UIControlStateNormal];
                                                    }];

    UIAlertAction *greenTeam = [UIAlertAction actionWithTitle:@"GREEN"
                                                        style:UIAlertActionStyleDefault
                                                      handler:^(UIAlertAction *action){

                                                          self.selectedPlayer = 3;
                                                          [self.selectTeamButton setTitle:@"GREEN" forState:UIControlStateNormal];
                                                      }];

    UIAlertAction *yellowTeam = [UIAlertAction actionWithTitle:@"YELLOW"
                                                         style:UIAlertActionStyleDefault
                                                       handler:^(UIAlertAction *action){

                                                           self.selectedPlayer = 4;
                                                           [self.selectTeamButton setTitle:@"YELLOW" forState:UIControlStateNormal];
                                                       }];

    NSIndexPath *selectedPath = self.mapPoolTableView.indexPathForSelectedRow;

    if (selectedPath == nil) {
        return;
    }

    NSInteger players = [self.data[@(selectedPath.row)][@"players"] integerValue];

    switch (players) {
        case 2: {

            [selectTeamActionViewController addAction:blueTeam];
            [selectTeamActionViewController addAction:redTeam];
            [selectTeamActionViewController addAction:cancelAction];
        }
            break;
        case 3: {

            [selectTeamActionViewController addAction:blueTeam];
            [selectTeamActionViewController addAction:redTeam];
            [selectTeamActionViewController addAction:greenTeam];
            [selectTeamActionViewController addAction:cancelAction];

        }
            break;
        case 4: {

            [selectTeamActionViewController addAction:blueTeam];
            [selectTeamActionViewController addAction:redTeam];
            [selectTeamActionViewController addAction:greenTeam];
            [selectTeamActionViewController addAction:yellowTeam];
            [selectTeamActionViewController addAction:cancelAction];
        }
            break;
        default: {

        }
            break;
    }

    selectTeamActionViewController.view.tintColor = [UIColor colorWithRed:0.96 green:0.72 blue:0.19 alpha:1.0];

    if ([UIDevice currentDevice].userInterfaceIdiom == UIUserInterfaceIdiomPad) {

        selectTeamActionViewController.popoverPresentationController.delegate = self;
    }

    [self presentViewController:selectTeamActionViewController
                       animated:YES
                     completion:nil];
}

- (void)prepareForPopoverPresentation:(UIPopoverPresentationController *)popoverPresentationController {

    popoverPresentationController.sourceView = self.selectTeamButton;
    popoverPresentationController.sourceRect = self.selectTeamButton.bounds;
}

- (IBAction)backButtonAction:(id)sender {

    [self.navigationController popViewControllerAnimated:YES];
}

- (nonnull UITableViewCell *)tableView:(nonnull UITableView *)tableView
                 cellForRowAtIndexPath:(nonnull NSIndexPath *)indexPath {

    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:@"BasicTableViewCell"
                                                            forIndexPath:indexPath];

    NSDictionary *data = self.data[@(indexPath.row)];

    cell.textLabel.text = data[@"name"];
    cell.textLabel.backgroundColor = [UIColor clearColor];

    UIView *bgView = [[UIView alloc] init];
    bgView.backgroundColor = [UIColor colorWithRed:0.96 green:0.72 blue:0.19 alpha:1.0];
    cell.selectedBackgroundView = bgView;

    return cell;
}

- (NSInteger)tableView:(nonnull UITableView *)tableView numberOfRowsInSection:(NSInteger)section {

    return self.data.allKeys.count;
}

- (CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath {

    return 44;
}

- (void)tableView:(UITableView *)tableView willDisplayCell:(UITableViewCell *)cell forRowAtIndexPath:(NSIndexPath *)indexPath {

    cell.backgroundColor = [UIColor clearColor];
    cell.contentView.backgroundColor = [UIColor clearColor];
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath {

    NSDictionary *data = self.data[@(indexPath.row)];

    self.previewMapImage.image = [UIImage imageNamed:data[@"image"]];
    self.playersPlaying = [self.data[@(indexPath.row)][@"players"] integerValue];
    self.selectedPlayer = 1;

    [self.selectTeamButton setTitle:@"BLUE" forState:UIControlStateNormal];
}

@end
