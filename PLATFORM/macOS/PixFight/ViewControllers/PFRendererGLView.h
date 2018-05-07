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
@property (weak) NSButton *undoTurnButton;

- (void)setGameLogic:(GameLogic *)gameLogic;
- (void)startAnimating;
- (void)stopAnimating;

- (void)undo;

@end
