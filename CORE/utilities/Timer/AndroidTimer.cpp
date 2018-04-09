//
//  AndroidTimer.cpp
//  PixFight
//
//  Created by Marcin Małysz on 11/02/2018.
//  Copyright © 2018 Marcin Małysz. All rights reserved.
//

#include "AndroidTimer.hpp"

#include <time.h>

void AndroidTimer::update() {

    timespec lTimeVal;

    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &lTimeVal);

    this->m_startTime = lTimeVal.tv_sec + (lTimeVal.tv_nsec * 1.0e-9);
    m_elapsed = (m_startTime - m_lastTime);
    m_lastTime = m_startTime;
}

double AndroidTimer::getElapsedSeconds() {

    return m_elapsed;
}
