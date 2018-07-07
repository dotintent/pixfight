//
//  PFMultiplayerViewController.m
//  PixFight
//
//  Created by Marcin Małysz on 26/06/2018.
//  Copyright © 2018 Marcin Małysz. All rights reserved.
//

#import "PFMultiplayerViewController.h"
#import "PFRoomViewController.h"
#import "PFJoinViewController.h"
#import "NSView+DisableActions.h"

#import "PFMultiplayerClient.hpp"

using namespace std;

@interface PFMultiplayerViewController () {

    shared_ptr<PFMultiplayerClient> _client;
}

@property (nonatomic, assign) BOOL connected;
@property (weak) IBOutlet NSButton *makeRoomButton;
@property (weak) IBOutlet NSButton *joinRoomButton;
@property (weak) IBOutlet NSButton *privateSwitch;

@end

@implementation PFMultiplayerViewController

- (void)viewDidLoad {
    [super viewDidLoad];

    _client = make_shared<PFMultiplayerClient>(DEFAULT_SERVER_ADDR);
}

- (void)viewDidAppear {
    [super viewDidAppear];

    if (!self.connected) {

        self.connected = _client->connect();
    }

    if (!self.connected) {

        NSAlert *alert = [[NSAlert alloc] init];

        [alert setMessageText:@"Connection Error"];
        [alert setInformativeText:@"Could not connect to server, please try again."];
        [alert setAlertStyle:NSInformationalAlertStyle];
        [alert addButtonWithTitle:@"Ok"];

        NSModalResponse response = [alert runModal];

        if (response == 1000) {

            [self dismissController:nil];
        }

        return;
    }

    __weak __typeof__(self) weakSelf = self;

    _client->callback = [=](const PFSocketCommandType command, const vector<uint8_t> data){

        if (command != PFSocketCommandTypeMakeRoom) {
            return;
        }

        dispatch_async(dispatch_get_main_queue(), ^{


            [weakSelf performSegueWithIdentifier:@"PFRoomSegueIdentifier" sender:weakSelf];
        });
    };

    self.makeRoomButton.enabled = YES;
    self.joinRoomButton.enabled = YES;
    self.privateSwitch.enabled = YES;
}

- (void)viewWillAppear {
    [super viewWillAppear];

    if (self.privateSwitch.hidden) {

        [self dismissController:nil];
        return;
    }

    if (self.connected) {

        [self.view enableSubViews];
    }
}

- (void)viewWillDisappear {
    [super viewWillDisappear];
    [self.view disableSubViews];
}

- (IBAction)backAction:(id)sender {

    [self dismissController:nil];
}

- (IBAction)makeRoomAction:(NSButton *)sender {

    [sender setState:NSControlStateValueOff];

    BOOL isPrivate = self.privateSwitch.state == NSOnState;
    _client->makeRoom(isPrivate);
}

- (IBAction)joinRoomAction:(NSButton *)sender {

    [sender setState:NSControlStateValueOff];
    [self performSegueWithIdentifier:@"PFJoinSegueIdentfier" sender:self];
}

- (void)prepareForSegue:(NSStoryboardSegue *)segue sender:(id)sender {

    self.privateSwitch.hidden = YES;

    if ([segue.identifier isEqualToString:@"PFRoomSegueIdentifier"]) {

        PFRoomViewController *roomVC = segue.destinationController;
        roomVC.master = YES;
        roomVC.client = _client;
    }
    else if ([segue.identifier isEqualToString:@"PFJoinSegueIdentfier"]) {

        PFJoinViewController *joinVC = segue.destinationController;
        joinVC.client = _client;
    }
}

@end
