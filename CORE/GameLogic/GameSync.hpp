//
//  GameSync.hpp
//  PixFight
//
//  Created by Marcin Małysz on 21/01/2018.
//  Copyright © 2018 Marcin Małysz. All rights reserved.
//

#pragma once

#include <functional>
#include <queue>
#include <mutex>
#include <future>

class GameLogic;

extern std::queue<std::function<void(void *constex, GameLogic *sender)>> syncQueue;
extern std::mutex syncMutex;
extern void syncToMainLoop(std::function<void(void*, GameLogic*)> callback);
