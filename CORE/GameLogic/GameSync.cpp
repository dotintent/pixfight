//
//  GameSync.cpp
//  PixFight
//
//  Created by Marcin Małysz on 21/01/2018.
//  Copyright © 2018 Marcin Małysz. All rights reserved.
//

#include "GameSync.hpp"

std::queue<std::function<void(void*context, GameLogic*sender)>> syncQueue;
std::mutex syncMutex;
std::atomic_bool action(true);

void syncToMainLoop(std::function<void(void*, GameLogic*)> callback) {

    syncMutex.lock();
    syncQueue.push(callback);
    syncMutex.unlock();
}
