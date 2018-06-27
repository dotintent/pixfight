//
//  PFMultiplayerViewController.m
//  PixFight
//
//  Created by Marcin Małysz on 17/06/2018.
//  Copyright © 2018 Marcin Małysz. All rights reserved.
//

#import "PFMultiplayerViewController.h"
#import "PFMultiplayerClient.hpp"

#import "PFJoinViewController.h"
#import "PFRoomViewController.h"

using namespace std;

@interface PFMultiplayerViewController () {

    shared_ptr<PFMultiplayerClient> _client;
}

@property (nonatomic, assign) BOOL connected;

@property (nonatomic, weak) IBOutlet NSLayoutConstraint *backgroundEqualWidthLayoutConstraint;
@property (nonatomic, weak) IBOutlet UIButton *makeRoomButton;
@property (nonatomic, weak) IBOutlet UIButton *joinRoomButton;
@property (nonatomic, weak) IBOutlet UISwitch *privateSwitch;

@end

@implementation PFMultiplayerViewController

- (void)viewDidLoad {

    [super viewDidLoad];

    if ([UIDevice currentDevice].userInterfaceIdiom == UIUserInterfaceIdiomPad) {

        [NSLayoutConstraint deactivateConstraints:@[self.backgroundEqualWidthLayoutConstraint]];
        [self.view layoutIfNeeded];
    }

    _client = make_shared<PFMultiplayerClient>(DEFAULT_SERVER_ADDR);
}

- (void)viewDidAppear:(BOOL)animated {
    [super viewDidAppear:animated];

    if (!self.connected) {

        self.connected = _client->connect();
    }

    if (!self.connected) {

        UIAlertController *alert = [UIAlertController alertControllerWithTitle:@"Error"
                                                                       message:@"Could not connect to server, please try again."
                                                                preferredStyle:UIAlertControllerStyleAlert];

        UIAlertAction* noButton = [UIAlertAction actionWithTitle:@"OK"
                                                           style:UIAlertActionStyleDefault
                                                         handler:^(UIAlertAction * action) {

                                                             [self backAction:nil];
                                                         }];

        [alert addAction:noButton];

        [self presentViewController:alert animated:YES completion:nil];
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

- (IBAction)backAction:(UIButton *)sender {

    [self.navigationController popViewControllerAnimated:YES];
}

- (IBAction)makeRoomAction:(UIButton *)sender {

    _client->makeRoom(self.privateSwitch.on);
}

- (IBAction)joinRoomAction:(UIButton *)sender {

    [self performSegueWithIdentifier:@"PFJoinSegueIdentfier" sender:self];
}

- (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender {

    if ([segue.identifier isEqualToString:@"PFRoomSegueIdentifier"]) {

        PFRoomViewController *roomVC = segue.destinationViewController;
        roomVC.master = YES;
        roomVC.client = _client;
    }
    else if ([segue.identifier isEqualToString:@"PFJoinSegueIdentfier"]) {

        PFJoinViewController *joinVC = segue.destinationViewController;
        joinVC.client = _client;
    }
}

@end
