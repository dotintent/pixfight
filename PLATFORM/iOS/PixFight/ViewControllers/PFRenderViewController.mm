//
//  PFRenderViewController.m
//  PixFight
//
//  Created by Marcin Małysz on 06/01/2018.
//  Copyright © 2018 Marcin Małysz. All rights reserved.
//

#import "Core-pch.hpp"
#import "FurionGL.hpp"
#import "AppDelegate+Audio.h"
#import "GameLogic.hpp"

#import "PFRenderViewController.h"
#import "PFSelectUnitViewController.h"

@interface PFRenderViewController () <PFSelectUnitViewControllerDelegate, UIPopoverPresentationControllerDelegate> {

    GameLogic *gameLogic;
    GameBase *selectedBase;
}

@property (nonatomic, weak) GLKView *glView;
@property (nonatomic, strong) EAGLContext *context;

@property (nonatomic, weak) IBOutlet UIButton *endTurnButton;
@property (nonatomic, weak) IBOutlet UIButton *menuButton;
@property (nonatomic, weak) IBOutlet UIButton *undoButton;
@property (nonatomic, weak) IBOutlet UIButton *multiplyButton;
@property (nonatomic, weak) IBOutlet UIVisualEffectView *blurView;

@property (nonatomic, assign) NSInteger selectedPlayer;
@property (nonatomic, assign) NSInteger playersCount;
@property (nonatomic, copy) NSString *mapName;
@property (nonatomic, copy) NSString *savePath;
@property (nonatomic, assign) BOOL allowInteraction;

@end

@implementation PFRenderViewController

- (void)viewDidLoad {
    
    [super viewDidLoad];

    gameLogic = nullptr;
    selectedBase = nullptr;

    self.blurView.alpha = 0.3;

    self.context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
    
    if (!self.context) {
        NSLog(@"Failed to create ES context");
    }
    
    self.glView = (GLKView *)self.view;
    self.glView.context = self.context;
    self.glView.drawableDepthFormat = GLKViewDrawableDepthFormat24;
    self.preferredFramesPerSecond = 120;
    
    self.glView.delegate = self;
    self.view.backgroundColor = [UIColor whiteColor];
    self.view.contentScaleFactor = 1.0;

    self.glView.drawableMultisample = GLKViewDrawableMultisampleNone;

    self.paused = false;

    UITapGestureRecognizer *tapGesture = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(tapGestureRecognizerAction:)];
    UIPanGestureRecognizer *panGesture = [[UIPanGestureRecognizer alloc] initWithTarget:self action:@selector(panGestureRecognizerAction:)];
    panGesture.maximumNumberOfTouches = 1;
    UIPinchGestureRecognizer *pinchGesture = [[UIPinchGestureRecognizer alloc] initWithTarget:self action:@selector(pinchGestureRecognizerAction:)];
  
    [self.view addGestureRecognizer:tapGesture];
    [self.view addGestureRecognizer:panGesture];
    [self.view addGestureRecognizer:pinchGesture];

    [EAGLContext setCurrentContext:self.context];
}

- (void)dealloc {

    AppDelegate *delegate = (AppDelegate *)[UIApplication sharedApplication].delegate;
    [delegate loadMusicNamed:@"menu"];

    NSLog(@"Render DEALLOC");
    
    if (gameLogic) {

        delete gameLogic;
        gameLogic = nullptr;
    }

    if ([EAGLContext currentContext] == self.context) {
        [EAGLContext setCurrentContext:nil];
    }
}

- (void)viewWillAppear:(BOOL)animated {
    [super viewWillAppear:animated];

    if (!gameLogic) {

        [self setupGame];
    }
}

- (void)prepareForPopoverPresentation:(UIPopoverPresentationController *)popoverPresentationController {

    popoverPresentationController.sourceView = self.menuButton;
    popoverPresentationController.sourceRect = self.menuButton.bounds;
}

- (void)setupGame {

    std::string rootPath = [[NSBundle mainBundle] resourcePath].UTF8String;
                rootPath += "/";

    if (gameLogic) {
        delete gameLogic;
    }

    AppDelegate *delegate = (AppDelegate *)[UIApplication sharedApplication].delegate;
    auto unit = [delegate audioUnit];

    [delegate loadMusicNamed:@"map"];
    
    gameLogic = new GameLogic(CGRectGetWidth(self.view.bounds),
                              CGRectGetHeight(self.view.bounds),
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
}

- (void)disconnectAction {

    UIAlertController *alert =  [UIAlertController alertControllerWithTitle:@"ERROR"
                                                                    message:@"You have been disconnected from the game..."
                                                             preferredStyle:UIAlertControllerStyleAlert];

    UIAlertAction* ok = [UIAlertAction actionWithTitle:@"OK"
                                                 style:UIAlertActionStyleDefault
                                               handler:^(UIAlertAction * action) {

                                                   [alert dismissViewControllerAnimated:YES
                                                                             completion:^{

                                                                                 [self.navigationController popToRootViewControllerAnimated:YES];
                                                                             }];
                                               }];

    [alert addAction:ok];

    [self presentViewController:alert
                       animated:YES
                     completion:nil];
}

- (void)mutliplayerWinAction:(NSInteger)playerID {

    if (self.presentedViewController) {

        [self dismissViewControllerAnimated:NO completion:nil];
    }

    UIAlertController *alert =  [UIAlertController alertControllerWithTitle:@"GAME FINISED"
                                                                    message:[NSString stringWithFormat:@"Player %ld win!", (long)playerID]
                                                             preferredStyle:UIAlertControllerStyleAlert];

    UIAlertAction* ok = [UIAlertAction actionWithTitle:@"OK"
                                                 style:UIAlertActionStyleDefault
                                               handler:^(UIAlertAction * action) {

                                                   [alert dismissViewControllerAnimated:YES
                                                                             completion:^{

                                                                                 [self.navigationController popToRootViewControllerAnimated:YES];

                                                                             }];
                                               }];

    [alert addAction:ok];

    [self presentViewController:alert
                       animated:YES
                     completion:nil];
}

- (void)multiplayerTurnAction {

    UIAlertController *alert =  [UIAlertController alertControllerWithTitle:@"INFO"
                                                                    message:@"It's your turn!"
                                                             preferredStyle:UIAlertControllerStyleAlert];

    UIAlertAction* ok = [UIAlertAction actionWithTitle:@"OK"
                                                 style:UIAlertActionStyleDefault
                                               handler:^(UIAlertAction * action) {

                                                   [alert dismissViewControllerAnimated:YES
                                                                             completion:nil];
                                               }];

    [alert addAction:ok];

    [self presentViewController:alert
                       animated:YES
                     completion:nil];
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

- (IBAction)undoAction:(id)sender {

    if (!gameLogic) {
        return;
    }

    gameLogic->undo();
}

- (IBAction)endTurnAction:(id)sender {

    if (!gameLogic) {
        return;
    }

    gameLogic->endTurn();
}

- (IBAction)homeAction:(id)sender {

    if (self.client) {

        self.client->disconnect();
        [self.navigationController popToRootViewControllerAnimated:YES];

        return;
    }

    UIAlertController *menuAlert = [UIAlertController alertControllerWithTitle:@"MENU"
                                                                       message:nil
                                                                preferredStyle:UIAlertControllerStyleActionSheet];

    UIAlertAction *cancelAction = [UIAlertAction actionWithTitle:@"Cancel"
                                                           style:UIAlertActionStyleCancel
                                                         handler:nil];

    UIAlertAction *quitAction = [UIAlertAction actionWithTitle:@"QUIT"
                                                         style:UIAlertActionStyleDefault
                                                       handler:^(UIAlertAction * _Nonnull action) {

                                                           [self.navigationController popToRootViewControllerAnimated:YES];
                                                       }];

    UIAlertAction *saveAction = [UIAlertAction actionWithTitle:@"SAVE"
                                                         style:UIAlertActionStyleDefault
                                                       handler:^(UIAlertAction * _Nonnull action) {

                                                           [self saveGame];
                                                       }];

    [menuAlert addAction:quitAction];
    [menuAlert addAction:saveAction];
    [menuAlert addAction:cancelAction];

    menuAlert.view.tintColor = [UIColor colorWithRed:0.96 green:0.72 blue:0.19 alpha:1.0];

    if ([UIDevice currentDevice].userInterfaceIdiom == UIUserInterfaceIdiomPad) {

        menuAlert.popoverPresentationController.delegate = self;
    }

    [self presentViewController:menuAlert
                       animated:YES
                     completion:nil];
}

- (IBAction)multipleTimeAction:(UIButton *)sender {

    if (!gameLogic) {
        return;
    }

    int time = (int)gameLogic->multiplyTime();

    [sender setTitle:[NSString stringWithFormat:@"X%d", time]
            forState:UIControlStateNormal];
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

    UIAlertController *alert =  [UIAlertController alertControllerWithTitle:info
                                                                    message:result
                                                             preferredStyle:UIAlertControllerStyleAlert];

    UIAlertAction* ok = [UIAlertAction actionWithTitle:@"OK"
                                                 style:UIAlertActionStyleDefault
                                               handler:^(UIAlertAction * action) {

                                                   [alert dismissViewControllerAnimated:YES
                                                                             completion:nil];
                                               }];

    [alert addAction:ok];

    [self presentViewController:alert
                       animated:YES
                     completion:nil];

}

- (void)winGameAction {

    UIAlertController *alert =  [UIAlertController alertControllerWithTitle:@"GAME FINISED"
                                                                    message:@"You win the game!"
                                                             preferredStyle:UIAlertControllerStyleAlert];

    UIAlertAction* ok = [UIAlertAction actionWithTitle:@"OK"
                                                 style:UIAlertActionStyleDefault
                                               handler:^(UIAlertAction * action) {

                             [alert dismissViewControllerAnimated:YES
                                                       completion:nil];
                         }];

    [alert addAction:ok];

    [self presentViewController:alert
                       animated:YES
                     completion:nil];
}

- (void)loseGameAction {

    UIAlertController *alert =  [UIAlertController alertControllerWithTitle:@"GAME FINISED"
                                                                    message:@"Sorry you lose!"
                                                             preferredStyle:UIAlertControllerStyleAlert];

    UIAlertAction* ok = [UIAlertAction actionWithTitle:@"OK"
                                                 style:UIAlertActionStyleDefault
                                               handler:^(UIAlertAction * action) {

                                                   [alert dismissViewControllerAnimated:YES
                                                                             completion:nil];
                                               }];

    [alert addAction:ok];

    [self presentViewController:alert
                       animated:YES
                     completion:nil];

}

- (void)botsBeginRoundAction {

    self.blurView.hidden = false;
}

- (void)botsEndRoundAction {

    self.blurView.hidden = true;
}

- (void)selectedMapAction:(GameBase *)map {

    selectedBase = map;
    [self performSegueWithIdentifier:@"PFBuildUnitSegueIdentifier" sender:self];
}

- (void)tapGestureRecognizerAction:(UITapGestureRecognizer *)recognizer {

    if (!gameLogic) {
        return;
    }

    if (self.allowInteraction == NO) {
        return;
    }

    CGPoint tapPoint = [recognizer locationInView:self.view];

    gameLogic->touchDownAtPoint(xVec2(tapPoint.x, tapPoint.y));
}

- (void)panGestureRecognizerAction:(UIPanGestureRecognizer *)recognizer {

    if (!gameLogic) {
        return;
    }

    CGPoint vel = [recognizer velocityInView:self.view];

    gameLogic->setDirectionVec(xVec2(-vel.x, -vel.y));
}

- (void)pinchGestureRecognizerAction:(UIPinchGestureRecognizer *)recognizer {

    if (!gameLogic) {
        return;
    }

    static float lastScale = 1.0f;
    float scale = [recognizer scale];
    float currentScale = lastScale * scale;
    lastScale = scale;

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

- (void)glkView:(GLKView *)view drawInRect:(CGRect)rect {

    if (!gameLogic) {
        return;
    }

    gameLogic->Render();

    self.endTurnButton.enabled = gameLogic->canEndTurn();
    self.undoButton.enabled = gameLogic->canUndo() && gameLogic->canEndTurn();
}

- (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender {

    PFSelectUnitViewController *selectUnit = segue.destinationViewController;

    selectUnit.delegate = self;
    [selectUnit startWithTeamID:selectedBase->getTeamID()
                    playerMoney:gameLogic->getPlayerCash()];

}

- (void)selectUnitView:(PFSelectUnitViewController *)sender
   didFinishWithUnitID:(int)unitId {

    gameLogic->buildNewUnitFromBase(selectedBase, unitId, [sender remainedCash]);
}

@end
