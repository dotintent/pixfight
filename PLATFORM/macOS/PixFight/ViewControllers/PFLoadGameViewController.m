//
//  PFLoadGameViewController.m
//  PixFight
//
//  Created by Marcin Małysz on 27/01/2018.
//  Copyright © 2018 Marcin Małysz. All rights reserved.
//

#import "PFLoadGameViewController.h"
#import "PFRenderViewController.h"

#import "NSView+DisableActions.h"

@interface PFLoadGameViewController ()<NSTableViewDelegate, NSTableViewDataSource>

@property (weak) IBOutlet NSTableView *savesTableView;
@property (weak) IBOutlet NSButton *loadButton;
@property (weak) IBOutlet NSButton *deleteButton;

@property (strong) NSMutableArray<NSString *> *paths;
@property (copy) NSString *savePathDirectory;
@property (copy) NSString *selectedPath;

@end

@implementation PFLoadGameViewController

- (void)viewDidLoad { //CellIdentifier
    [super viewDidLoad];

    self.savesTableView.delegate = self;
    self.savesTableView.dataSource = self;

    NSURL *documents = [[[NSFileManager defaultManager] URLsForDirectory:NSDocumentDirectory
                                                               inDomains:NSUserDomainMask] lastObject];
    NSString *path = [documents path];
    path = [path stringByAppendingPathComponent:@"save"];

    self.savePathDirectory = path;
    NSFileManager *manager = [NSFileManager defaultManager];

    if ([manager fileExistsAtPath:path]) {

        self.paths = [[manager contentsOfDirectoryAtPath:path error:nil] mutableCopy];

        if (!self.paths) {
            self.paths = [NSMutableArray array];
        }
    }

    [self.savesTableView reloadData];
}

- (void)viewWillAppear {
    [super viewWillAppear];

    if (self.savesTableView.hidden) {

        [self dismissController:nil];
    }

    self.savesTableView.hidden = NO;
    [self.view enableSubViews];

    self.deleteButton.enabled = NO;
    self.loadButton.enabled = NO;
}

- (void)viewWillDisappear {
    [super viewWillDisappear];
    [self.view disableSubViews];
}

- (IBAction)backAction:(id)sender {

    [self dismissController:nil];
}

- (IBAction)loadGame:(id)sender {

    [sender setState:NSControlStateValueOff];
    [self performSegueWithIdentifier:@"PFRenderSegueIdentifier" sender:self];
}

- (IBAction)deleteSave:(id)sender {

    [sender setState:NSControlStateValueOff];

    self.deleteButton.enabled = NO;
    self.loadButton.enabled = NO;

    NSInteger row = self.savesTableView.selectedRow;

    if (row < 0) {
        return;
    }

    NSString *path = self.paths[row];
    NSError *error = nil;

    NSString *removePath = [self.savePathDirectory stringByAppendingPathComponent:path];

    if (![[NSFileManager defaultManager] removeItemAtPath:removePath error:&error]) {

        NSLog(@"Could not remove file at path: %@ error: %@", path, error.localizedDescription);

        return;
    }

    [self.paths removeObjectAtIndex:row];
    [self.savesTableView reloadData];
}

- (void)prepareForSegue:(NSStoryboardSegue *)segue sender:(id)sender {

    if ([segue.identifier isEqualToString:@"PFRenderSegueIdentifier"]) {

        self.savesTableView.hidden = YES;

        PFRenderViewController *controller = (PFRenderViewController *)segue.destinationController;
        [controller loadGamePath:self.selectedPath];
    }
}

- (NSInteger)numberOfRowsInTableView:(NSTableView *)tableView {

    return self.paths.count;
}

- (nullable id)tableView:(NSTableView *)tableView objectValueForTableColumn:(nullable NSTableColumn *)tableColumn row:(NSInteger)row {

    return self.paths[row];
}

- (void)tableViewSelectionDidChange:(NSNotification *)notification {

    NSInteger row = self.savesTableView.selectedRow;

    if (row < 0) {
        self.deleteButton.enabled = NO;
        self.loadButton.enabled = NO;
        return;
    }

    self.deleteButton.enabled = self.paths.count > 0;
    self.loadButton.enabled = self.paths.count > 0;
    self.selectedPath = [self.savePathDirectory stringByAppendingPathComponent:self.paths[row]];
}

@end
