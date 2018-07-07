//
//  PFJoinViewController.m
//  PixFight
//
//  Created by Marcin Małysz on 27/06/2018.
//  Copyright © 2018 Marcin Małysz. All rights reserved.
//

#import "PFJoinViewController.h"
#import "PFRoomViewController.h"

#import "NSView+DisableActions.h"

using namespace std;

@interface PFJoinViewController ()<NSTableViewDelegate, NSTableViewDataSource>

@property (weak) IBOutlet NSButton *refreshButton;
@property (weak) IBOutlet NSButton *joinButton;
@property (weak) IBOutlet NSTextField *codeTextField;
@property (weak) IBOutlet NSTableView *roomsTableView;

@property (nonatomic, strong) NSMutableArray *rooms;
@property (nonatomic, strong) NSDateFormatter *timeFormatter;

@end

@implementation PFJoinViewController

- (void)viewDidLoad {
    [super viewDidLoad];

    self.rooms = @[].mutableCopy;

    self.timeFormatter = [[NSDateFormatter alloc] init];
    [self.timeFormatter setDateFormat:@"MM/dd/yyyy HH:mm"];

    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(textDidChange:) name:NSControlTextDidChangeNotification
                                               object:self.codeTextField];

    self.roomsTableView.delegate = self;
    self.roomsTableView.dataSource = self;
}

- (void)viewWillAppear {
    [super viewWillAppear];

    if (self.roomsTableView.hidden) {

        [self dismissController:nil];
    }

    [self.view enableSubViews];
}

- (void)viewWillDisappear {
    [super viewWillDisappear];
    [self.view disableSubViews];
}

- (void)viewDidAppear {

    [super viewDidAppear];

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

- (void)textDidChange:(NSNotification *)notification {

    NSTextField *textField = [[notification userInfo] valueForKey:@"NSFieldEditor"];
    [self textFieldDidChange:textField];
}

- (void)textFieldDidChange:(NSTextField *)textField {

    self.joinButton.enabled = textField.stringValue.length > 0;
}

- (IBAction)backAction:(NSButton *)sender {

    [self dismissController:nil];
}

- (IBAction)joinAction:(NSButton *)sender {

    uint32_t roomID = (uint32_t)[self.codeTextField.stringValue integerValue];

    if (_client->joinRoom(roomID)) {

        self.roomsTableView.hidden = YES;
        [self performSegueWithIdentifier:@"PFRoomSegueIdentifier" sender:self];
        return;
    }

    NSAlert *alert = [[NSAlert alloc] init];

    [alert setMessageText:@"Error"];
    [alert setInformativeText:@"Could not connect to room."];
    [alert setAlertStyle:NSInformationalAlertStyle];
    [alert addButtonWithTitle:@"Ok"];

    NSModalResponse response = [alert runModal];

    if (response == 1000) {

        [self dismissController:nil];
    }
}

- (IBAction)refreshAction:(NSButton *)sender {

    [sender setState:NSControlStateValueOff];

    [self.rooms removeAllObjects];
    [self.refreshButton setEnabled:NO];
    _client->listRooms();
}

- (void)prepareForSegue:(NSStoryboardSegue *)segue sender:(id)sender {

    if ([segue.identifier isEqualToString:@"PFRoomSegueIdentifier"]) {

        PFRoomViewController *roomVC = segue.destinationController;
        roomVC.master = NO;
        roomVC.client = _client;
    }
}

- (NSInteger)numberOfRowsInTableView:(NSTableView *)tableView {

    return self.rooms.count;
}

- (nullable id)tableView:(NSTableView *)tableView objectValueForTableColumn:(nullable NSTableColumn *)tableColumn
                     row:(NSInteger)row {

    NSDictionary *data = self.rooms[row];

    return [self.timeFormatter stringFromDate:data[@"created"]];
}

- (void)tableViewSelectionDidChange:(NSNotification *)notification {

    NSInteger row = self.roomsTableView.selectedRow;

    if (row < 0) {
        self.joinButton.enabled = NO;
        return;
    }

    NSDictionary *data = self.rooms[row];

    self.codeTextField.stringValue = [NSString stringWithFormat:@"%@", data[@"port"]];
    [self textFieldDidChange:self.codeTextField];
}

@end
