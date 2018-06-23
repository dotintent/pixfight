//
//  PFRoomViewController.m
//  PixFight
//
//  Created by Marcin Małysz on 17/06/2018.
//  Copyright © 2018 Marcin Małysz. All rights reserved.
//

#import "PFRoomViewController.h"
#import "PFRenderViewController.h"

using namespace std;

@interface PFRoomViewController () <UIPopoverPresentationControllerDelegate>

@property (nonatomic, weak) IBOutlet NSLayoutConstraint *backgroundEqualWidthLayoutConstraint;
@property (nonatomic, weak) IBOutlet UIButton *mapButton;
@property (nonatomic, weak) IBOutlet UIButton *readyButton;
@property (nonatomic, weak) IBOutlet UIImageView *mapImage;
@property (nonatomic, weak) IBOutlet UILabel *infoLabel;
@property (nonatomic, strong) NSDictionary *data;
@property (nonatomic, assign) NSInteger currentPlayerID;

@end

@implementation PFRoomViewController

- (void)viewDidLoad {
    [super viewDidLoad];

    if ([UIDevice currentDevice].userInterfaceIdiom == UIUserInterfaceIdiomPad) {

        [NSLayoutConstraint deactivateConstraints:@[self.backgroundEqualWidthLayoutConstraint]];
        [self.view layoutIfNeeded];
    }

    self.data = @{

                  @(0) : @{
                          @"image" : @"tutorial.png",
                          @"mapname" : @"tutorial",
                          @"players" : @(2)
                          },
                  @(1) : @{
                          @"image" : @"Hunt.png",
                          @"mapname" : @"Hunt",
                          @"players" : @(3)
                          },
                  @(2) : @{
                          @"image" : @"pasage.png",
                          @"mapname" : @"pasage",
                          @"players" : @(2)
                          },
                  @(3) : @{
                          @"image" : @"waterway.png",
                          @"mapname" : @"waterway",
                          @"players" : @(2)
                          },
                  @(4) : @{
                          @"image" : @"kingofhill.png",
                          @"mapname" : @"kingofhill",
                          @"players" : @(4)
                          },
                  };
}

- (void)prepareForPopoverPresentation:(UIPopoverPresentationController *)popoverPresentationController {

    popoverPresentationController.sourceView = self.mapButton;
    popoverPresentationController.sourceRect = self.mapButton.bounds;
}

- (void)viewWillAppear:(BOOL)animated {
    [super viewWillAppear:animated];

    self.mapButton.enabled = self.isMaster;
    self.infoLabel.text = [NSString stringWithFormat:@"Room ID: %d", self.client->getCurrentPort()];
}

- (void)viewDidAppear:(BOOL)animated {

    [super viewDidAppear:animated];

    __weak __typeof__(self) weakSelf = self;

    self.client->callback = [=](const PFSocketCommandType command, const vector<uint8_t> data) {

        dispatch_async(dispatch_get_main_queue(), ^{

            if (command == PFSocketCommandTypeGameInfo) {

                PFRoomInfo info;
                memcpy(&info, data.data(), data.size() * sizeof(uint8_t));

                _client->setRoomInfo(info);

                NSString *mapName = [NSString stringWithUTF8String:info.mapname];

                [weakSelf.mapButton setTitle:[mapName uppercaseString] forState:UIControlStateNormal];

                for (NSDictionary *val in weakSelf.data.allValues) {

                    if ([val[@"mapname"] isEqualToString:mapName]) {

                        [weakSelf.mapImage setImage:[UIImage imageNamed:val[@"image"]]];

                        break;
                    }
                }

                weakSelf.readyButton.enabled = YES;
            }

            if (command == PFSocketCommandTypeLoad) {

                uint32_t playerId = 0;
                memcpy(&playerId, data.data(), sizeof(uint32_t));
                weakSelf.currentPlayerID = playerId+1;
                [weakSelf performSegueWithIdentifier:@"PFStartSegueIdentifier" sender:weakSelf];
            }

            if (command == PFSocketCommandTypeDisconnect) {

                [weakSelf backAction:nil];
            }
        });
    };

    if (self.isMaster) {

        [self updateGameInfoForMap:@(0)];
    }
    else {

        self.client->getRoomDetails();
    }
}

- (IBAction)backAction:(UIButton *)sender {

    if (self.isMaster) {

        self.client->removeRoom();
    }
    else {

        self.client->leaveRoom();
    }

    [self.navigationController popToRootViewControllerAnimated:YES];
}

- (IBAction)readyAction:(UIButton *)sender {

    sender.enabled = NO;
    self.client->setReady();
}

- (IBAction)mapAction:(UIButton *)sender {

    UIAlertController *selectTeamActionViewController = [UIAlertController alertControllerWithTitle:@"Choose map"
                                                                                            message:nil
                                                                                     preferredStyle:UIAlertControllerStyleActionSheet];

    UIAlertAction *cancelAction = [UIAlertAction actionWithTitle:@"Cancel"
                                                           style:UIAlertActionStyleCancel
                                                         handler:nil];

    UIAlertAction *map1Action = [UIAlertAction actionWithTitle:@"TUTORIAL"
                                                         style:UIAlertActionStyleDefault
                                                       handler:^(UIAlertAction *action){

                                                           [self updateGameInfoForMap:@(0)];
                                                       }];

    UIAlertAction *map2Action = [UIAlertAction actionWithTitle:@"HUNT"
                                                         style:UIAlertActionStyleDefault
                                                       handler:^(UIAlertAction *action){

                                                           [self updateGameInfoForMap:@(1)];
                                                       }];

    UIAlertAction *map3Action = [UIAlertAction actionWithTitle:@"PASAGE"
                                                         style:UIAlertActionStyleDefault
                                                       handler:^(UIAlertAction *action){

                                                           [self updateGameInfoForMap:@(2)];
                                                       }];

    UIAlertAction *map4Action = [UIAlertAction actionWithTitle:@"WATERWAY"
                                                         style:UIAlertActionStyleDefault
                                                       handler:^(UIAlertAction *action){

                                                           [self updateGameInfoForMap:@(3)];
                                                       }];

    UIAlertAction *map5Action = [UIAlertAction actionWithTitle:@"KINGOFHILL"
                                                         style:UIAlertActionStyleDefault
                                                       handler:^(UIAlertAction *action){

                                                           [self updateGameInfoForMap:@(4)];
                                                       }];

    selectTeamActionViewController.view.tintColor = [UIColor colorWithRed:0.96 green:0.72 blue:0.19 alpha:1.0];

    [selectTeamActionViewController addAction:map1Action];
    [selectTeamActionViewController addAction:map2Action];
    [selectTeamActionViewController addAction:map3Action];
    [selectTeamActionViewController addAction:map4Action];
    [selectTeamActionViewController addAction:map5Action];
    [selectTeamActionViewController addAction:cancelAction];

    if ([UIDevice currentDevice].userInterfaceIdiom == UIUserInterfaceIdiomPad) {

        selectTeamActionViewController.popoverPresentationController.delegate = self;
    }

    [self presentViewController:selectTeamActionViewController
                       animated:YES
                     completion:nil];
}

- (void)updateGameInfoForMap:(NSNumber *)mapId {

    NSDictionary *dict = self.data[mapId];

    NSString *mapname = dict[@"mapname"];

    [self.mapButton setTitle:mapname forState:UIControlStateNormal];
    [self.mapImage setImage:[UIImage imageNamed:dict[@"image"]]];

    PFRoomInfo roomInfo;

    strcpy(roomInfo.mapname, mapname.UTF8String);
    roomInfo.players = [dict[@"players"] integerValue];
    roomInfo.createdDate = time(0);
    roomInfo.roomPort = self.client->getCurrentPort();

    self.client->setRoomInfo(roomInfo);
    self.client->sendRoomDetails();

    self.readyButton.enabled = YES;
}

- (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender {

    if ([segue.identifier isEqualToString:@"PFStartSegueIdentifier"]) {

        PFRenderViewController *gameController = segue.destinationViewController;

        gameController.client = self.client;

        PFRoomInfo info = self.client->getRoomInfo();

        [gameController startGame:[NSString stringWithUTF8String:info.mapname]
                         playerID:self.currentPlayerID
                      playersSize:info.players];
    }
}

@end
