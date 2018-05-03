//
//  PFSettingsViewController.m
//  PixFight
//
//  Created by Marcin Małysz on 24/01/2018.
//  Copyright © 2018 Marcin Małysz. All rights reserved.
//

#import "PFSettingsViewController.h"
#import "AppDelegate+Audio.h"

@interface PFSettingsViewController ()
@property (weak) IBOutlet NSButton *muteSwitch;
@property (weak) IBOutlet NSButton *hardAISwitch;
@property (weak) IBOutlet NSSlider *volumeSlider;
@property (weak) AppDelegate *delegate;
@end

@implementation PFSettingsViewController

- (void)viewDidLoad {
    [super viewDidLoad];

    self.delegate = (AppDelegate *)[[NSApplication sharedApplication] delegate];

    self.preferredContentSize = NSMakeSize(400, 200);

    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];

    BOOL isMuted = [defaults boolForKey:@"mute"];
    BOOL isHardAI = [defaults boolForKey:@"hardai"];

    [self.muteSwitch setState: isMuted ? NSControlStateValueOn : NSControlStateValueOff];
    [self.hardAISwitch setState: isHardAI ? NSControlStateValueOn : NSControlStateValueOff];

    Audio *audio = [self.delegate audioUnit];

    self.volumeSlider.floatValue = audio->getVolume();
}

- (IBAction)dimissAction:(id)sender {

    [self dismissViewController:self];
}

- (IBAction)muteAction:(NSButton *)sender {

    BOOL isOn = sender.state == NSControlStateValueOn;

    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];

    [defaults setBool:isOn forKey:@"mute"];
    [defaults synchronize];

    Audio *audio = [self.delegate audioUnit];

    if (isOn) {

        audio->mute();
    }
    else {

        audio->unmute();
    }
}

- (IBAction)hardAIAction:(NSButton *)sender {

    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];

    [defaults setBool:sender.state == NSControlStateValueOn forKey:@"hardai"];
    [defaults synchronize];
}

- (IBAction)volumeChangeAction:(NSSlider *)sender {

    float volume = sender.floatValue;

    Audio *audio = [self.delegate audioUnit];

    audio->setVolume(volume);
}

@end
