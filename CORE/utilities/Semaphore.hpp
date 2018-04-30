//
//  Semaphore.hpp
//  PixFight
//
//  Created by Marcin Małysz on 01/05/2018.
//  Copyright © 2018 Marcin Małysz. All rights reserved.
//

#pragma once

#include <mutex>
#include <condition_variable>

class Semaphore {

public:

    Semaphore (int count = 0) : _count(count) {}
    ~Semaphore() = default;

    inline void notify() {

        std::unique_lock<std::mutex> lock(_mtx);
        _count++;
        _cv.notify_one();
    }

    inline void wait() {

        std::unique_lock<std::mutex> lock(_mtx);

        while(_count == 0){
            _cv.wait(lock);
        }

        _count--;
    }

private:

    std::mutex _mtx;
    std::condition_variable _cv;
    int _count;
};
