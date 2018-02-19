//
//  PFSelectUnitViewController.m
//  PixFight
//
//  Created by Marcin Małysz on 22/01/2018.
//  Copyright © 2018 Marcin Małysz. All rights reserved.
//

#import "PFSelectUnitViewController.h"

@interface PFSelectUnitViewController () <UITableViewDelegate, UITableViewDataSource>

@property (nonatomic, weak) IBOutlet UILabel *cashLabel;
@property (nonatomic, weak) IBOutlet UITableView *unitsTableView;
@property (nonatomic, strong) NSDictionary *data;

@property (nonatomic, assign) int currentMoney;
@property (nonatomic, assign) int teamID;

@end

@implementation PFSelectUnitViewController

- (void)viewDidLoad {

    [super viewDidLoad];

    self.data = @{
        @(0) : @{
            @"image" : @"icon_infantry",
            @"name" : @"Infantry",
            @"cost" : @(75)
        },

        @(1) : @{
            @"image" : @"icon_bazooka",
            @"name" : @"Bazooka",
            @"cost" : @(150)
        },

        @(2) : @{
            @"image" : @"icon_jeep",
            @"name" : @"Jeep",
            @"cost" : @(200)
        },

        @(3) : @{
            @"image" : @"icon_tank",
            @"name" : @"Tank",
            @"cost" : @(300)
        },

        @(4) : @{
            @"image" : @"icon_artillery",
            @"name" : @"Artillery",
            @"cost" : @(200)
        },
    };
}

- (void)startWithTeamID:(int)teamID
            playerMoney:(int)availableMoney {

    self.teamID = teamID;
    self.currentMoney = availableMoney;
}

- (void)viewWillAppear:(BOOL)animated {
    [super viewWillAppear:animated];

    self.cashLabel.text = [NSString stringWithFormat:@"Cash: %@", @(self.currentMoney)];
    self.unitsTableView.delegate = self;
    self.unitsTableView.dataSource = self;

    [self.unitsTableView reloadData];
}

- (int)remainedCash {

    return self.currentMoney;
}

- (IBAction)closeAction:(id)sender {

    [self dismissViewControllerAnimated:YES completion:nil];
}

- (nonnull UITableViewCell *)tableView:(nonnull UITableView *)tableView cellForRowAtIndexPath:(nonnull NSIndexPath *)indexPath {

    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:@"UnitCellIdentifier"
                                                            forIndexPath:indexPath];

    NSDictionary *cellData = self.data[@(indexPath.row)];

    NSInteger cost = [cellData[@"cost"] integerValue];

    cell.textLabel.text = [NSString stringWithFormat:@"%@ - %@", cellData[@"name"], @(cost)];
    cell.imageView.image = [UIImage imageNamed:[NSString stringWithFormat:@"%@%@.png", cellData[@"image"], @(self.teamID)]];

    cell.textLabel.textColor = cost > self.currentMoney ? [UIColor redColor] : [UIColor blackColor];

    UIView *selectionView = [[UIView alloc] init];
    selectionView.backgroundColor = [UIColor colorWithRed:0.96 green:0.72 blue:0.19 alpha:1.0];

    cell.selectedBackgroundView = selectionView;

    return cell;
}

- (NSIndexPath *)tableView:(UITableView *)tableView willSelectRowAtIndexPath:(NSIndexPath *)indexPath {

    NSDictionary *cellData = self.data[@(indexPath.row)];
    NSInteger cost = [cellData[@"cost"] integerValue];

    return cost > self.currentMoney ? nil : indexPath;
}

- (NSInteger)tableView:(nonnull UITableView *)tableView numberOfRowsInSection:(NSInteger)section {

    return self.data.count;
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath {

    NSDictionary *cellData = self.data[@(indexPath.row)];

    self.currentMoney -= (int)[cellData[@"cost"] integerValue];

    int unitID = (int)indexPath.row;
    [self.delegate selectUnitView:self didFinishWithUnitID:unitID];

    [self dismissViewControllerAnimated:YES completion:nil];
}

@end
