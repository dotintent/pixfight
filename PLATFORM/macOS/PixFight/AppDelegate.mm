//
//  AppDelegate.m
//  PixFight
//
//  Created by Marcin Małysz on 23/01/2018.
//  Copyright © 2018 Marcin Małysz. All rights reserved.
//

#import "AppDelegate.h"
#import "AppDelegate+Audio.h"

@interface AppDelegate () {

    Audio *audioUnit;
    Audio::SoundID selectSound;
}

@end

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {

    [self initializeAudioUnit];
}

- (void)applicationWillTerminate:(NSNotification *)aNotification {

    if (audioUnit) {

        delete audioUnit;
    }
}

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)sender {

    return YES;
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

    audioUnit->setVolume(0.1);

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

@end
