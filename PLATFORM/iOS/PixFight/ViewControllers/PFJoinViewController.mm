//
//  PFJoinViewController.m
//  PixFight
//
//  Created by Marcin Małysz on 17/06/2018.
//  Copyright © 2018 Marcin Małysz. All rights reserved.
//

#import "PFJoinViewController.h"
#import "PFRoomViewController.h"

using namespace std;

@interface PFJoinViewController () <UITableViewDelegate, UITableViewDataSource>

@property (nonatomic, weak) IBOutlet NSLayoutConstraint *backgroundEqualWidthLayoutConstraint;
@property (nonatomic, weak) IBOutlet UIButton *refreshButton;
@property (nonatomic, weak) IBOutlet UIButton *joinButton;
@property (nonatomic, weak) IBOutlet UITextField *codeTextField;
@property (nonatomic, weak) IBOutlet UITableView *roomsTableView;

@property (nonatomic, strong) NSMutableArray *rooms;
@property (nonatomic, strong) NSDateFormatter *timeFormatter;

@end

@implementation PFJoinViewController

- (void)viewDidLoad {

    [super viewDidLoad];

    self.rooms = @[].mutableCopy;

    if ([UIDevice currentDevice].userInterfaceIdiom == UIUserInterfaceIdiomPad) {

        [NSLayoutConstraint deactivateConstraints:@[self.backgroundEqualWidthLayoutConstraint]];
        [self.view layoutIfNeeded];
    }

    self.timeFormatter = [[NSDateFormatter alloc] init];
    [self.timeFormatter setDateFormat:@"MM/dd/yyyy HH:mm"];

    [self.codeTextField addTarget:self
                           action:@selector(textFieldDidChange:)
                 forControlEvents:UIControlEventEditingChanged];

    self.roomsTableView.delegate = self;
    self.roomsTableView.dataSource = self;
}

- (void)viewDidAppear:(BOOL)animated {

    [super viewDidAppear:animated];

    __weak __typeof__(self) weakSelf = self;

    _client->callback = [=](const PFSocketCommandType command, const vector<uint8_t> data) {

        if (command != PFSocketCommandTypeRooms) {
            return;
        }

        dispatch_async(dispatch_get_main_queue(), ^{

            size_t roomsSize = (data.size() * sizeof(uint8_t)) / sizeof(PFRoomInfo);

            vector<PFRoomInfo> rooms(roomsSize);

            memcpy(rooms.data(), data.data(), data.size() * sizeof(uint8_t));

            for (int i = 0; i < rooms.size(); ++i) {

                auto room = rooms[i];

                [weakSelf.rooms addObject:({

                    NSDictionary *dict = @{
                                           @"name" : [NSString stringWithUTF8String:room.mapname],
                                           @"players" : @(room.players),
                                           @"created" : [NSDate dateWithTimeIntervalSince1970:room.createdDate],
                                           @"port" : @(room.roomPort)
                                           };

                    dict;
                })];
            }

            [weakSelf.roomsTableView reloadData];
            [weakSelf.refreshButton setEnabled:YES];
        });
    };

    _client->listRooms();
}

- (void)textFieldDidChange:(UITextField *)textField {

    self.joinButton.enabled = textField.text.length > 0;
}

- (IBAction)backAction:(UIButton *)sender {

    [self.navigationController popToRootViewControllerAnimated:YES];
}

- (IBAction)joinAction:(UIButton *)sender {

    uint32_t roomID = (uint32_t)[self.codeTextField.text integerValue];

    if (_client->joinRoom(roomID)) {

        [self performSegueWithIdentifier:@"PFRoomSegueIdentifier" sender:self];
        return;
    }

    UIAlertController *alert = [UIAlertController alertControllerWithTitle:@"Error"
                                                                   message:@"Could not connect to room."
                                                            preferredStyle:UIAlertControllerStyleAlert];

    UIAlertAction* noButton = [UIAlertAction actionWithTitle:@"OK"
                                                       style:UIAlertActionStyleDefault
                                                     handler:^(UIAlertAction * action) {

                                                         [self backAction:nil];
                                                     }];

    [alert addAction:noButton];

    [self presentViewController:alert animated:YES completion:nil];
}

- (IBAction)refreshAction:(UIButton *)sender {

    [self.rooms removeAllObjects];
    [self.refreshButton setEnabled:NO];
    _client->listRooms();
}

- (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender {

    if ([segue.identifier isEqualToString:@"PFRoomSegueIdentifier"]) {

        PFRoomViewController *roomVC = segue.destinationViewController;
        roomVC.master = NO;
        roomVC.client = _client;
    }
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath {

    NSDictionary *data = self.rooms[indexPath.row];

    self.codeTextField.text = [NSString stringWithFormat:@"%@", data[@"port"]];
    [self textFieldDidChange:self.codeTextField];
}

- (nonnull UITableViewCell *)tableView:(nonnull UITableView *)tableView cellForRowAtIndexPath:(nonnull NSIndexPath *)indexPath {

    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:@"PFRoomCellIdentifier"
                                                            forIndexPath:indexPath];

    NSDictionary *data = self.rooms[indexPath.row];

    NSString *date = [self.timeFormatter stringFromDate:data[@"created"]];
    NSString *map = data[@"name"];

    cell.textLabel.text = [NSString stringWithFormat:@"%@ - %@", map, date];

    return cell;
}

- (NSInteger)tableView:(nonnull UITableView *)tableView numberOfRowsInSection:(NSInteger)section {

    return self.rooms.count;
}


@end
