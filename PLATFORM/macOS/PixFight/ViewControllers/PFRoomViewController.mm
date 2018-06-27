//
//  PFRoomViewController.m
//  PixFight
//
//  Created by Marcin Małysz on 26/06/2018.
//  Copyright © 2018 Marcin Małysz. All rights reserved.
//

#import "PFRoomViewController.h"
#import "PFRenderViewController.h"
#import "PFSelectMapController.h"

#import "NSView+DisableActions.h"

using namespace std;

@interface PFRoomViewController () <PFSelectMapControllerDelegate>

@property (weak) IBOutlet NSButton *mapButton;
@property (weak) IBOutlet NSButton *readyButton;
@property (weak) IBOutlet NSImageView *mapImage;
@property (weak) IBOutlet NSTextField *infoLabel;
@property (nonatomic, strong) NSDictionary *data;
@property (nonatomic, assign) NSInteger currentPlayerID;

@end

@implementation PFRoomViewController

- (void)viewDidLoad {
    [super viewDidLoad];

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

- (void)viewWillAppear {
    [super viewWillAppear];

    if (self.mapImage.hidden) {
        [self dismissController:nil];
    }

    self.mapButton.enabled = self.isMaster;
    self.infoLabel.stringValue = [NSString stringWithFormat:@"Room ID: %d", self.client->getCurrentPort()];
}

- (void)viewWillDisappear {
    [super viewWillDisappear];
    [self.view disableSubViews];
}

- (void)viewDidAppear {
    [super viewDidAppear];

    __weak __typeof__(self) weakSelf = self;

    self.client->callback = [=](const PFSocketCommandType command, const vector<uint8_t> data) {

        dispatch_async(dispatch_get_main_queue(), ^{

            if (command == PFSocketCommandTypeGameInfo) {

                PFRoomInfo info;
                memcpy(&info, data.data(), data.size() * sizeof(uint8_t));

                _client->setRoomInfo(info);

                NSString *mapName = [NSString stringWithUTF8String:info.mapname];

                [weakSelf.mapButton setTitle:[mapName uppercaseString]];

                for (NSDictionary *val in weakSelf.data.allValues) {

                    if ([val[@"mapname"] isEqualToString:mapName]) {

                        [weakSelf.mapImage setImage:[NSImage imageNamed:val[@"image"]]];

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

- (IBAction)backAction:(id)sender {

    if (self.isMaster) {

        self.client->removeRoom();
    }
    else {

        self.client->leaveRoom();
    }

    [self dismissController:nil];
}

- (IBAction)readyAction:(NSButton *)sender {

    sender.enabled = NO;
    self.client->setReady();
}

- (IBAction)mapAction:(NSButton *)sender {

    [self performSegueWithIdentifier:@"PFSelectMapSegueIdentifier" sender:self];
}

- (void)updateGameInfoForMap:(NSNumber *)mapId {

    NSDictionary *dict = self.data[mapId];

    NSString *mapname = dict[@"mapname"];

    [self.mapButton setTitle:mapname];
    [self.mapImage setImage:[NSImage imageNamed:dict[@"image"]]];

    PFRoomInfo roomInfo;

    strcpy(roomInfo.mapname, mapname.UTF8String);
    roomInfo.players = [dict[@"players"] integerValue];
    roomInfo.createdDate = time(0);
    roomInfo.roomPort = self.client->getCurrentPort();

    self.client->setRoomInfo(roomInfo);
    self.client->sendRoomDetails();

    self.readyButton.enabled = YES;
}

- (void)prepareForSegue:(NSStoryboardSegue *)segue sender:(id)sender {

    if ([segue.identifier isEqualToString:@"PFStartSegueIdentifier"]) {

        self.mapImage.hidden = YES;

        PFRenderViewController *gameController = segue.destinationController;

        gameController.client = self.client;

        PFRoomInfo info = self.client->getRoomInfo();

        [gameController startGame:[NSString stringWithUTF8String:info.mapname]
                         playerID:self.currentPlayerID
                      playersSize:info.players];
    }
    else if([segue.identifier isEqualToString:@"PFSelectMapSegueIdentifier"]) {

        PFSelectMapController *selectController = segue.destinationController;
        selectController.delegate = self;
    }
}

- (void)selectMapController:(PFSelectMapController *)sender didSelectMap:(NSNumber *)mapNumber {

    [self updateGameInfoForMap:mapNumber];
    [sender dismissController:nil];
}

@end
