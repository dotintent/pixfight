//
//  PFRendererGLView.h
//  PixFight
//
//  Created by Marcin Małysz on 25/01/2018.
//  Copyright © 2018 Marcin Małysz. All rights reserved.
//

#import "Core-pch.hpp"
#import "GameLogic.hpp"

@interface PFRendererGLView : NSOpenGLView

@property (weak) NSButton *endRoundButton;

- (void)setGameLogic:(GameLogic *)gameLogic;
- (void)startAnimating;
- (void)stopAnimating;

@end
