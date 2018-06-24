//
//  AppDelegate.m
//  PixFight
//
//  Created by Marcin Małysz on 01/01/2018.
//  Copyright © 2018 Marcin Małysz. All rights reserved.
//

#import <UserNotifications/UserNotifications.h>

#import <AVFoundation/AVFoundation.h>
#import "AppDelegate.h"
#import "AppDelegate+Audio.h"

@interface AppDelegate ()<UNUserNotificationCenterDelegate> {
    
    Audio *audioUnit;
    Audio::SoundID selectSound;
}

@end

@implementation AppDelegate

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    // Override point for customization after application launch.

    UNAuthorizationOptions options = UNAuthorizationOptionAlert + UNAuthorizationOptionSound;

    UNUserNotificationCenter *center = [UNUserNotificationCenter currentNotificationCenter];

    center.delegate = self;

    [center requestAuthorizationWithOptions:options
                          completionHandler:^(BOOL granted, NSError * _Nullable error) {

                              if (!granted) {

                                  NSLog(@"Something went wrong");
                              }
                          }];

    application.applicationSupportsShakeToEdit = NO;
    
    [self setupAudioSession];
    [self initializeAudioUnit];

    return YES;
}

- (void)userNotificationCenter:(UNUserNotificationCenter *)center
       willPresentNotification:(UNNotification *)notification
         withCompletionHandler:(void (^)(UNNotificationPresentationOptions))completionHandler {

    completionHandler(UNNotificationPresentationOptionSound | UNNotificationPresentationOptionAlert | UNNotificationPresentationOptionBadge);
}

- (void)applicationWillResignActive:(UIApplication *)application {
}

- (void)applicationDidEnterBackground:(UIApplication *)application {
}

- (void)applicationWillEnterForeground:(UIApplication *)application {
}

- (void)applicationDidBecomeActive:(UIApplication *)application {
}

- (void)applicationWillTerminate:(UIApplication *)application {

    delete audioUnit;
}

- (Audio *)audioUnit {
    
    return audioUnit;
}

- (void)playSelectSound {
    
    audioUnit->playSound(selectSound);
}

- (void)initializeAudioUnit {
    
    audioUnit = new Audio();
    
    NSBundle *main = [NSBundle mainBundle];
    
    NSString *sound2 = [main pathForResource:@"select" ofType:@"wav"];
    
    selectSound = audioUnit->loadSound(sound2.UTF8String);
    
    audioUnit->setVolume(1.0);
    
    [self loadMusicNamed:@"menu"];
    
    bool muted = [[NSUserDefaults standardUserDefaults] boolForKey:@"mute"];
    
    if (muted) {
 
        audioUnit->mute();
    }
}

- (void)loadMusicNamed:(NSString *)name {
    
    audioUnit->pauseMusic();
    NSBundle *main = [NSBundle mainBundle];
    NSString *menuMusic = [main pathForResource:name ofType:@"mp3"];
    audioUnit->loadMusic(menuMusic.UTF8String);
    audioUnit->playMusic();
}

- (void)setupAudioSession {
    
    double rate = 44100.0;
    int blockSize = 512;
    long channels = 2;
    BOOL success = false;
    
    AVAudioSession *session = [AVAudioSession sharedInstance];
    
    // Make our category 'solo' for the best chance at getting our desired settings
    // Use AVAudioSessionCategoryPlayAndRecord if you need microphone input
    success = [session setCategory:AVAudioSessionCategorySoloAmbient error:nil];
    assert(success);
    
    // Set our preferred rate and activate the session to test it
    success = [session setPreferredSampleRate:rate error:nil];
    assert(success);
    success = [session setActive:TRUE error:nil];
    assert(success);
    
    // Query the actual supported rate and max channels
    rate = [session sampleRate];
    channels = [session respondsToSelector:@selector(maximumOutputNumberOfChannels)] ? [session maximumOutputNumberOfChannels] : 2;
    
    // Deactivate the session so we can change parameters without route changes each time
    success = [session setActive:FALSE error:nil];
    assert(success);
    
    // Set the duration and channels based on known supported values
    success = [session setPreferredIOBufferDuration:blockSize / rate error:nil];
    assert(success);
    if ([session respondsToSelector:@selector(setPreferredOutputNumberOfChannels:error:)])
    {
        success = [session setPreferredOutputNumberOfChannels:channels error:nil];
        assert(success);
    }
    
    /*
     Set up some observers for various notifications
     */
    [[NSNotificationCenter defaultCenter] addObserverForName:AVAudioSessionInterruptionNotification object:nil queue:nil usingBlock:^(NSNotification *notification)
     {
         bool began = [[notification.userInfo valueForKey:AVAudioSessionInterruptionTypeKey] intValue] == AVAudioSessionInterruptionTypeBegan;
         NSLog(@"Interruption %@", began ? @"Began" : @"Ended");
         
         if (!began)
         {
             [[AVAudioSession sharedInstance] setActive:TRUE error:nil];
         }
         
         //TODO: interruption
     }];
    
    [[NSNotificationCenter defaultCenter] addObserverForName:AVAudioSessionSilenceSecondaryAudioHintNotification object:nil queue:nil usingBlock:^(NSNotification *notification)
     {
         bool began = [[notification.userInfo valueForKey:AVAudioSessionSilenceSecondaryAudioHintTypeKey] intValue] == AVAudioSessionSilenceSecondaryAudioHintTypeBegin;
         NSLog(@"Silence secondary audio %@", began ? @"Began" : @"Ended");
     }];
    
    [[NSNotificationCenter defaultCenter] addObserverForName:AVAudioSessionRouteChangeNotification object:nil queue:nil usingBlock:^(NSNotification *notification)
     {
         NSNumber * reason = [[notification userInfo] valueForKey:AVAudioSessionRouteChangeReasonKey];
         
         AVAudioSessionRouteDescription * desc = [[notification userInfo] valueForKey:AVAudioSessionRouteChangePreviousRouteKey];
         
         AVAudioSessionPortDescription * oldOutput = [desc outputs][0];
         AVAudioSessionPortDescription * newOutput = [[[AVAudioSession sharedInstance] currentRoute] outputs][0];
         
         const char *reasonString = NULL;
         switch ([reason intValue])
         {
             case AVAudioSessionRouteChangeReasonNewDeviceAvailable:         reasonString = "New Device Available";              break;
             case AVAudioSessionRouteChangeReasonOldDeviceUnavailable:       reasonString = "Old Device Unavailable";            break;
             case AVAudioSessionRouteChangeReasonCategoryChange:             reasonString = "Category Change";                   break;
             case AVAudioSessionRouteChangeReasonOverride:                   reasonString = "Override";                          break;
             case AVAudioSessionRouteChangeReasonWakeFromSleep:              reasonString = "Wake From Sleep";                   break;
             case AVAudioSessionRouteChangeReasonNoSuitableRouteForCategory: reasonString = "No Suitable Route For Category";    break;
             case AVAudioSessionRouteChangeReasonRouteConfigurationChange:   reasonString = "Configuration Change";              break;
             default:                                                        reasonString = "Unknown";
         }
         
         NSLog(@"Output route has changed from %dch %@ to %dch %@ due to '%s'", (int)[[oldOutput channels] count], [oldOutput portName], (int)[[newOutput channels] count], [newOutput portName], reasonString);
     }];
    
    [[NSNotificationCenter defaultCenter] addObserverForName:AVAudioSessionMediaServicesWereLostNotification object:nil queue:nil usingBlock:^(NSNotification *notification)
     {
         NSLog(@"Media services were lost");
     }];
    
    [[NSNotificationCenter defaultCenter] addObserverForName:AVAudioSessionMediaServicesWereResetNotification object:nil queue:nil usingBlock:^(NSNotification *notification)
     {
         NSLog(@"Media services were reset");
     }];
    
    [[NSNotificationCenter defaultCenter] addObserverForName:UIApplicationDidBecomeActiveNotification object:nil queue:nil usingBlock:^(NSNotification *notification)
     {
         NSLog(@"Application did become active");
     }];
    [[NSNotificationCenter defaultCenter] addObserverForName:UIApplicationWillResignActiveNotification object:nil queue:nil usingBlock:^(NSNotification *notification)
     {
         NSLog(@"Application will resign active");
     }];
    [[NSNotificationCenter defaultCenter] addObserverForName:UIApplicationDidEnterBackgroundNotification object:nil queue:nil usingBlock:^(NSNotification *notification)
     {
         NSLog(@"Application did enter background");
     }];
    [[NSNotificationCenter defaultCenter] addObserverForName:UIApplicationWillEnterForegroundNotification object:nil queue:nil usingBlock:^(NSNotification *notification)
     {
         NSLog(@"Application will enter foreground");
     }];
    
    
    /*
     Activate the audio session
     */
    success = [session setActive:TRUE error:nil];
    assert(success);
}

@end

