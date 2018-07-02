#pragma once

#include <functional>
#include <queue>
#include <mutex>
#include <future>

extern std::queue<std::function<void(void)>> runLoopQueue;
extern std::mutex runLoopMutex;
extern void syncToRunLoop(std::function<void(void)> callback);
