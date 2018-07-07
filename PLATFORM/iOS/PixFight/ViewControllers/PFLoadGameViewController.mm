//
//  PFLoadGameViewController.m
//  PixFight
//
//  Created by Marcin Małysz on 06/01/2018.
//  Copyright © 2018 Marcin Małysz. All rights reserved.
//

#import "PFLoadGameViewController.h"
#import "PFRenderViewController.h"

@interface PFLoadGameViewController ()<UITableViewDelegate, UITableViewDataSource>

@property (nonatomic, weak) IBOutlet UITableView *savesTableView;
@property (nonatomic, weak) IBOutlet NSLayoutConstraint *backgroundEqualWidthLayoutConstraint;
@property (nonatomic, strong) NSArray<NSString *> *paths;
@property (nonatomic, copy) NSString *savePathDirectory;
@property (nonatomic, copy) NSString *selectedPath;

@end

@implementation PFLoadGameViewController

- (void)viewDidLoad {
    [super viewDidLoad];

    if ([UIDevice currentDevice].userInterfaceIdiom == UIUserInterfaceIdiomPad) {

        [NSLayoutConstraint deactivateConstraints:@[self.backgroundEqualWidthLayoutConstraint]];
        [self.view layoutIfNeeded];
    }
}

- (void)viewWillAppear:(BOOL)animated {
    [super viewWillAppear:animated];

    self.savesTableView.delegate = self;
    self.savesTableView.dataSource = self;

    NSURL *documents = [[[NSFileManager defaultManager] URLsForDirectory:NSDocumentDirectory
                                                               inDomains:NSUserDomainMask] lastObject];
    NSString *path = [documents path];
    path = [path stringByAppendingPathComponent:@"save"];

    self.savePathDirectory = path;
    NSFileManager *manager = [NSFileManager defaultManager];

    if ([manager fileExistsAtPath:path]) {

        self.paths = [manager contentsOfDirectoryAtPath:path error:nil];
    }

    [self.savesTableView reloadData];
}

- (IBAction)backButtonAction:(id)sender {

    [self.navigationController popViewControllerAnimated:YES];
}

- (BOOL)tableView:(UITableView *)tableView canMoveRowAtIndexPath:(NSIndexPath *)indexPath {

    return NO;
}

- (void)tableView:(UITableView *)tableView commitEditingStyle:(UITableViewCellEditingStyle)editingStyle forRowAtIndexPath:(NSIndexPath *)indexPath {

    if (editingStyle != UITableViewCellEditingStyleDelete) {
        return;
    }

    NSString *path = self.paths[indexPath.row];
    NSError *error = nil;

    NSString *removePath = [self.savePathDirectory stringByAppendingPathComponent:path];

    if (![[NSFileManager defaultManager] removeItemAtPath:removePath error:&error]) {

        NSLog(@"Could not remove file at path: %@ error: %@", path, error.localizedDescription);

        return;
    }

    NSMutableArray *updateArray = self.paths.mutableCopy;
    [updateArray removeObjectAtIndex:indexPath.row];
    self.paths = updateArray;

    [tableView reloadData];
}

- (nonnull UITableViewCell *)tableView:(nonnull UITableView *)tableView
                 cellForRowAtIndexPath:(nonnull NSIndexPath *)indexPath {

    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:@"SaveCellIdentifier"
                                                            forIndexPath:indexPath];


    NSString *path = self.paths[indexPath.row];

    cell.textLabel.text = path;

    return cell;
}

- (NSInteger)tableView:(nonnull UITableView *)tableView numberOfRowsInSection:(NSInteger)section {

    return self.paths.count;
}

- (CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath {

    return 32;
}

- (void)tableView:(UITableView *)tableView willDisplayCell:(UITableViewCell *)cell forRowAtIndexPath:(NSIndexPath *)indexPath {

    cell.backgroundColor = [UIColor clearColor];
    cell.contentView.backgroundColor = [UIColor clearColor];
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath {

    self.selectedPath = [self.savePathDirectory stringByAppendingPathComponent:self.paths[indexPath.row]];
    [self performSegueWithIdentifier:@"PFStartSegueIdentifier" sender:self];
}

- (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender {

    PFRenderViewController *controller = (PFRenderViewController *)segue.destinationViewController;

    [controller loadGamePath:self.selectedPath];
}

@end
