#include "ApplicationRunLoop.hpp"

std::queue<std::function<void(void)>> runLoopQueue;
std::mutex runLoopMutex;

void syncToRunLoop(std::function<void(void)> callback) {

    runLoopMutex.lock();
    runLoopQueue.push(callback);
    runLoopMutex.unlock();
}
