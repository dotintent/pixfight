//
//  PFRenderViewController.m
//  PixFight
//
//  Created by Marcin Małysz on 25/01/2018.
//  Copyright © 2018 Marcin Małysz. All rights reserved.
//

#import "PFRenderViewController.h"
#import "PFSelectUnitViewController.h"
#import "PFGameMenuViewController.h"
#import "PFRendererGLView.h"

#import "FurionGL.hpp"
#import "AppDelegate+Audio.h"
#import "GameLogic.hpp"
#import "NSView+DisableActions.h"

@interface PFRenderViewController ()
<PFSelectUnitViewControllerDelegate,
PFGameMenuViewControllerDelegate,
NSUserNotificationCenterDelegate> {

    GameLogic *gameLogic;
    NSEvent *_scrollKeyMonitor;
}

@property (assign) GameBase *selectedBase;
@property (weak) IBOutlet NSButton *endTurnButton;
@property (weak) IBOutlet NSButton *menuButton;
@property (weak) IBOutlet NSButton *undoButton;
@property (weak) IBOutlet NSButton *multiplyButton;
@property (weak) IBOutlet NSVisualEffectView *overlayView;
@property (weak) IBOutlet NSProgressIndicator *spinner;

@property (assign) NSInteger selectedPlayer;
@property (assign) NSInteger playersCount;
@property (copy) NSString *mapName;
@property (copy) NSString *savePath;
@property (assign) BOOL allowInteraction;

@property (strong) PFRendererGLView *renderView;

@end

@implementation PFRenderViewController

- (BOOL)userNotificationCenter:(NSUserNotificationCenter *)center shouldPresentNotification:(NSUserNotification *)notification {

    return YES;
}

- (void)viewDidLoad {
    [super viewDidLoad];

    self.overlayView.hidden = YES;
    self.overlayView.alphaValue = 0.3;
    [self.spinner startAnimation:nil];

    gameLogic = nullptr;
    self.selectedBase = nullptr;
}

- (void)dealloc {

    if (_scrollKeyMonitor) {
        [NSEvent removeMonitor:_scrollKeyMonitor];
        _scrollKeyMonitor = nil;
    }
    
    AppDelegate *delegate = (AppDelegate *)[NSApplication sharedApplication].delegate;
    [delegate loadMusicNamed:@"menu"];
    
    NSLog(@"Render DEALLOC");
    
    [self.renderView setGameLogic:nullptr];
    
    if (gameLogic) {
        
        delete gameLogic;
        gameLogic = nullptr;
    }
}

- (void)viewWillAppear {
    [super viewWillAppear];

    if (!gameLogic) {
        
        self.renderView = [[PFRendererGLView alloc] initWithFrame:self.view.bounds];
        [self.view addSubview:self.renderView positioned:NSWindowBelow relativeTo:self.menuButton];
        
        [[self.renderView openGLContext] makeCurrentContext];
        
        [self setupGame];
        
        __weak __typeof__(self) weakSelf = self;
        
        _scrollKeyMonitor = [NSEvent addLocalMonitorForEventsMatchingMask:NSScrollWheelMask
                                                                  handler:^(NSEvent *incomingEvent) {
                                                                      
                                                                      [weakSelf scrollEvent:incomingEvent];
                                                                      
                                                                      return incomingEvent;
                                                                  }];
    }
}

- (void)setupGame {
    
    NSClickGestureRecognizer *clickRecognizer = [[NSClickGestureRecognizer alloc] initWithTarget:self action:@selector(tapGestureAction:)];
    clickRecognizer.numberOfClicksRequired = 1;
    
    NSPanGestureRecognizer *panRecognizer = [[NSPanGestureRecognizer alloc] initWithTarget:self action:@selector(panGestureAction:)];
    panRecognizer.buttonMask = 0x2;
    
    [self.renderView addGestureRecognizer:clickRecognizer];
    [self.renderView addGestureRecognizer:panRecognizer];
    
    std::string rootPath = [[NSBundle mainBundle] resourcePath].UTF8String;
    rootPath += "/";
    
    if (gameLogic) {
        delete gameLogic;
    }
    
    AppDelegate *delegate = (AppDelegate *)[NSApplication sharedApplication].delegate;
    auto unit = [delegate audioUnit];
    
    [delegate loadMusicNamed:@"map"];
    
    gameLogic = new GameLogic(self.view.bounds.size.width,
                              self.view.bounds.size.height,
                              rootPath,
                              unit);
    
    gameLogic->context = (__bridge void*)self;
    
    //no need to use context we pass self in lambda
    gameLogic->winGameCallback = [](void* context) {
        
        __weak PFRenderViewController *controller = (__bridge PFRenderViewController *)context;
        [controller winGameAction];
    };
    
    gameLogic->loseGameCallback = [](void* context) {
        
        __weak PFRenderViewController *controller = (__bridge PFRenderViewController *)context;
        [controller loseGameAction];
    };
    
    gameLogic->botsStartThinkCallback = [](void* context) {
        
        __weak PFRenderViewController *controller = (__bridge PFRenderViewController *)context;
        [controller botsBeginRoundAction];
    };
    
    gameLogic->botsEndThinkCallback = [](void* context) {
        
        __weak PFRenderViewController *controller = (__bridge PFRenderViewController *)context;
        [controller botsEndRoundAction];
    };
    
    gameLogic->baseSelectedCallback = [](void* context, GameBase *base) {
        
        __weak PFRenderViewController *controller = (__bridge PFRenderViewController *)context;
        [controller selectedMapAction:base];
    };
    
    bool hardAI = [[NSUserDefaults standardUserDefaults] boolForKey:@"hardai"];
    gameLogic->setHardAI(hardAI);
    
    if (self.savePath) {
        
        gameLogic->loadGame(self.savePath.UTF8String);
    }
    else {
        
        int playerID = (int)self.selectedPlayer;
        int playerCount = (int)self.playersCount;
        
        gameLogic->createNewGame(self.mapName.UTF8String,
                                 playerID,
                                 playerCount,
                                 self.client);
    }

    if (self.client) {

        [self setupMultiplayer];
        [self updateMultiplayerState:0];

        self.client->setLoaded();
    }
    else {

        self.allowInteraction = YES;
    }

    self.renderView.endRoundButton = self.endTurnButton;
    self.renderView.undoTurnButton = self.undoButton;

    [self.renderView setGameLogic:gameLogic];
}

- (void)disconnectAction {

    NSAlert *alert = [[NSAlert alloc] init];

    [alert addButtonWithTitle:@"OK"];
    [alert setMessageText:@"ERROR"];
    [alert setInformativeText:@"You have been disconnected from the game..."];
    [alert setAlertStyle:NSInformationalAlertStyle];

    NSModalResponse response = [alert runModal];

    if (response == 1000) {

        [self dismissController:nil];
    }
}

- (void)mutliplayerWinAction:(NSInteger)playerID {

    NSAlert *alert = [[NSAlert alloc] init];

    [alert addButtonWithTitle:@"OK"];
    [alert setMessageText:@"GAME FINISED"];
    [alert setInformativeText:[NSString stringWithFormat:@"Player %ld win!", (long)playerID]];
    [alert setAlertStyle:NSInformationalAlertStyle];

    NSModalResponse response = [alert runModal];

    if (response == 1000) {

        [self dismissController:nil];
    }
}

- (void)multiplayerTurnAction {

    NSUserNotification *notification = [[NSUserNotification alloc] init];
    notification.identifier = @"pixfight_id";
    notification.title = @"Pixfight";
    notification.subtitle = @"It's your turn!";
    notification.soundName = NSUserNotificationDefaultSoundName;
    notification.contentImage = [NSApp applicationIconImage];

    NSUserNotificationCenter *center = [NSUserNotificationCenter defaultUserNotificationCenter];

    center.delegate = self;
    [center deliverNotification:notification];
    [center removeAllDeliveredNotifications];
}

- (void)setupMultiplayer {

    __weak __typeof__(self) weakSelf = self;
    
    self.client->callback = [=](const PFSocketCommandType cmd, const std::vector<uint8_t> pckt){

        PFSocketCommandType command = cmd;
        std::vector<uint8_t> data = pckt;

        dispatch_async(dispatch_get_main_queue(), ^{

            switch (command) {

                case PFSocketCommandTypeMakeRoom:
                case PFSocketCommandTypeLeaveRoom:
                case PFSocketCommandTypeRemoveRoom:
                case PFSocketCommandTypeGameInfo:
                case PFSocketCommandTypeGetGameInfo:
                case PFSocketCommandTypeUnknown:
                case PFSocketCommandTypeHeartbeat:
                case PFSocketCommandTypeReady:
                case PFSocketCommandTypeLoad:
                case PFSocketCommandTypeRooms:
                case PFSocketCommandTypeOk: {
                    break;
                }
                case PFSocketCommandTypeDisconnect: {

                    cout << "PFSocketCommandTypeDisconnect" << endl;

                    weakSelf.client->disconnect();
                    [weakSelf disconnectAction];

                    break;
                }
                case PFSocketCommandTypeSendTurn: {

                    uint32_t currentPlayerTurn = 0;

                    memcpy(&currentPlayerTurn, data.data(), data.size() * sizeof(uint8_t));

                    cout << "Current player turn: " << currentPlayerTurn << endl;

                    NSInteger playerID = currentPlayerTurn + 1;

                    [weakSelf updateMultiplayerState:playerID];

                    if (weakSelf.allowInteraction) {

                        gameLogic->startTurn();
                        [weakSelf multiplayerTurnAction];
                    }

                    break;
                }
                case PFSocketCommandTypeEndGame: {

                    uint32_t winnnerID = 0;
                    memcpy(&winnnerID, data.data(), data.size() * sizeof(uint8_t));

                    cout << "PFSocketCommandTypeEndGame" << endl;

                    weakSelf.client->disconnect();
                    gameLogic->startTurn();

                    [weakSelf mutliplayerWinAction:winnnerID];

                    break;
                }
                case PFSocketCommandTypeFire: {

                    uint32_t idA = 0;
                    uint32_t idB = 0;
                    uint32_t sizeA = 0;
                    uint32_t sizeB = 0;

                    memcpy(&idA, data.data(), sizeof(uint32_t));
                    memcpy(&idB, data.data() + sizeof(uint32_t), sizeof(uint32_t));
                    memcpy(&sizeA, data.data() + sizeof(uint32_t) * 2, sizeof(uint32_t));
                    memcpy(&sizeB, data.data() + sizeof(uint32_t) * 3, sizeof(uint32_t));

                    gameLogic->remoteAttackUnit(idA, idB, sizeA, sizeB);

                    break;
                }
                case PFSocketCommandTypeMove: {

                    uint32_t unitID = 0;
                    float posX = 0;
                    float posY = 0;

                    memcpy(&unitID, data.data(), sizeof(uint32_t));
                    memcpy(&posX, data.data() + sizeof(uint32_t), sizeof(float));
                    memcpy(&posY, data.data() + sizeof(uint32_t) + sizeof(float), sizeof(float));

                    gameLogic->remoteMoveUnit(unitID, posX, posY);

                    break;
                }
                case PFSocketCommandTypeBuild: {

                    uint32_t baseID = 0;
                    uint16_t unit = 0;

                    memcpy(&baseID, data.data(), sizeof(uint32_t));
                    memcpy(&unit, data.data() + sizeof(uint32_t), sizeof(uint16_t));

                    gameLogic->remoteBuildUnit(baseID, unit);

                    break;
                }
                case PFSocketCommandTypeCapture: {

                    uint32_t baseID = 0;
                    uint32_t unitID = 0;

                    memcpy(&baseID, data.data(), sizeof(uint32_t));
                    memcpy(&unitID, data.data() + sizeof(uint32_t), sizeof(uint32_t));

                    gameLogic->remoteCaptureBase(baseID, unitID);

                    break;
                }
                case PFSocketCommandTypeRepair: {

                    uint32_t baseID = 0;
                    uint32_t unitID = 0;

                    memcpy(&baseID, data.data(), sizeof(uint32_t));
                    memcpy(&unitID, data.data() + sizeof(uint32_t), sizeof(uint32_t));

                    gameLogic->remoteRepairUnit(baseID, unitID);

                    break;
                }

            }
        });
    };
}

- (void)updateMultiplayerState:(NSInteger)playerID {

    self.allowInteraction = (PLAYERTEAMSELECTED == playerID);

    self.endTurnButton.hidden = !self.allowInteraction;
    self.multiplyButton.hidden = YES;
    self.undoButton.hidden = YES;
}

- (IBAction)endTurnAction:(NSButton *)sender {
    
    [sender setState:NSControlStateValueOff];
    
    if (!gameLogic) {
        return;
    }
    
    gameLogic->endTurn();
}

- (IBAction)homeAction:(NSButton *)sender {

    [sender setState:NSControlStateValueOff];

    if (self.client) {

        self.client->callback = nullptr;
        self.client->disconnect();
        [self dismissController:nil];

        return;
    }

    [self performSegueWithIdentifier:@"PFMenuSegueIdentifier" sender:self];
}

- (IBAction)multipleTimeAction:(NSButton *)sender {
    
    [sender setState:NSControlStateValueOff];
    
    if (!gameLogic) {
        return;
    }
    
    int time = (int)gameLogic->multiplyTime();
    
    [sender setTitle:[NSString stringWithFormat:@"X%d", time]];
}

- (IBAction)undoAction:(NSButton *)sender {

    [sender setState:NSControlStateValueOff];

    [self.renderView undo];
}

- (void)saveGame {
    
    NSURL *documents = [[[NSFileManager defaultManager] URLsForDirectory:NSDocumentDirectory
                                                               inDomains:NSUserDomainMask] lastObject];
    NSString *path = [documents path];
    path = [path stringByAppendingPathComponent:@"save"];
    
    NSFileManager *manager = [NSFileManager defaultManager];
    
    if (![manager fileExistsAtPath:path]) {
        
        if (![manager createDirectoryAtPath:path
                withIntermediateDirectories:NO
                                 attributes:nil
                                      error:nil]) {
            
            NSLog(@"Could not create directory at path: %@", path);
        }
    }
    
    NSDateFormatter *dateFormatter = [[NSDateFormatter alloc] init];
    [dateFormatter setDateFormat:@"MM_dd_yyyy_HH_mm_ss"];
    
    NSString *formattedDateString = [dateFormatter stringFromDate:[NSDate date]];
    
    path = [path stringByAppendingPathComponent:[NSString stringWithFormat:@"%s_%@.sav",
                                                 gameLogic->getCurrentMapName().c_str(),
                                                 formattedDateString]];
    
    
    NSString *info = @"INFO";
    NSString *result = @"Game sucessfully saved";
    
    if (!gameLogic->saveGame(path.UTF8String)) {
        
        info = @"ERROR";
        result = @"Could not save game.";
    }

    NSAlert *alert = [[NSAlert alloc] init];

    [alert addButtonWithTitle:@"OK"];
    [alert setMessageText:info];
    [alert setInformativeText:result];
    [alert setAlertStyle:NSInformationalAlertStyle];

    [alert runModal];
}

- (void)winGameAction {

    dispatch_async(dispatch_get_main_queue(), ^{

        NSAlert *alert = [[NSAlert alloc] init];

        [alert addButtonWithTitle:@"OK"];
        [alert setMessageText:@"GAME END"];
        [alert setInformativeText:@"Congratulations, you win the game!"];
        [alert setAlertStyle:NSInformationalAlertStyle];

        [alert runModal];

    });
}

- (void)loseGameAction {

    dispatch_async(dispatch_get_main_queue(), ^{

        NSAlert *alert = [[NSAlert alloc] init];

        [alert addButtonWithTitle:@"OK"];
        [alert setMessageText:@"GAME END"];
        [alert setInformativeText:@"Sorry you lose!"];
        [alert setAlertStyle:NSInformationalAlertStyle];

        [alert runModal];
    });
}

- (void)botsBeginRoundAction {

    dispatch_async(dispatch_get_main_queue(), ^{

        self.overlayView.hidden = NO;
    });
}

- (void)botsEndRoundAction {

    dispatch_async(dispatch_get_main_queue(), ^{

        self.overlayView.hidden = YES;
    });
}

- (void)selectedMapAction:(GameBase *)map {
    
    self.selectedBase = map;

    dispatch_async(dispatch_get_main_queue(), ^{

        [self performSegueWithIdentifier:@"PFBuildUnitSegueIdentifier" sender:self];
    });
}

- (void)prepareForSegue:(NSStoryboardSegue *)segue sender:(id)sender {

    if ([segue.identifier isEqualToString:@"PFBuildUnitSegueIdentifier"]) {

        PFSelectUnitViewController *controller = (PFSelectUnitViewController *)segue.destinationController;
        controller.delegate = self;

        [controller startWithTeamID:self.selectedBase->getTeamID()
                        playerMoney:gameLogic->getPlayerCash()];
    }
    else if ([segue.identifier isEqualToString:@"PFMenuSegueIdentifier"]) {

        PFGameMenuViewController *controller = (PFGameMenuViewController *)segue.destinationController;
        controller.delegate = self;
    }
}

- (void)tapGestureAction:(NSClickGestureRecognizer *)recognizer {
    
    if (!gameLogic) {
        return;
    }

    if (!self.allowInteraction) {
        return;
    }

    NSPoint tapPoint = [recognizer locationInView:self.renderView];
    
    if(tapPoint.x < 0) tapPoint.x = 0;
    if(tapPoint.x > self.view.frame.size.width) tapPoint.x = self.view.frame.size.width;
    if(tapPoint.y < 0) tapPoint.y = 0;
    if(tapPoint.y > self.view.frame.size.height) tapPoint.y = self.view.frame.size.height;

    xVec2 pos = xVec2(tapPoint.x, self.view.frame.size.height - tapPoint.y);

    syncToMainLoop([pos](void *context, GameLogic *logic){

        logic->touchDownAtPoint(pos);
    });
}

- (void)panGestureAction:(NSPanGestureRecognizer *)recognizer {

    if (!gameLogic) {
        return;
    }

    CGPoint vel = [recognizer velocityInView:self.renderView];

    gameLogic->setDirectionVec(xVec2(vel.x, -vel.y));
}

- (void)scrollEvent:(NSEvent *)event {

    if (![self.renderView mouse:[event locationInWindow] inRect:self.view.frame]) {
        return;
    }
    
    if (!gameLogic) {
        return;
    }
    
    static float currentScale = 1.0;

    currentScale += ([event scrollingDeltaY] / 100.0f);
    if (currentScale < 0.25f) currentScale = 0.25;
    if (currentScale > 1.0f) currentScale = 1.0f;
    
    gameLogic->setCurrentScale(currentScale);
}

- (void)startGame:(NSString *)mapName
         playerID:(NSInteger)playerID
      playersSize:(NSInteger)playersCount {
    
    self.mapName = mapName;
    self.selectedPlayer = playerID;
    self.playersCount = playersCount;
    
    self.savePath = nil;
}

- (void)loadGamePath:(NSString *)filePath {
    
    self.savePath = filePath;
    
    self.mapName = nil;
    self.selectedPlayer = 0;
    self.playersCount = 0;
}

- (void)selectUnitView:(PFSelectUnitViewController *)sender
   didFinishWithUnitID:(int)unitId {

    int cash = [sender remainedCash];

    syncToMainLoop([unitId, cash](void *context, GameLogic *logic){

        __weak PFRenderViewController *controller = (__bridge PFRenderViewController *)context;

        logic->buildNewUnitFromBase(controller.selectedBase, unitId, cash);
    });
}

- (void)gameMenuViewControllerDidQuit:(PFGameMenuViewController *)sender {

    [self.renderView stopAnimating];
    [self.renderView setGameLogic:nullptr];

    [self dismissController:self];
}

- (void)gameMenuViewControllerDidSave:(PFGameMenuViewController *)sender {

    [self saveGame];
}

@end
