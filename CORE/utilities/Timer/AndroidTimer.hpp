//
//  AndroidTimer.hpp
//  PixFight
//
//  Created by Marcin Małysz on 11/02/2018.
//  Copyright © 2018 Marcin Małysz. All rights reserved.
//

#pragma once

#include "GameTimer.hpp"

class AndroidTimer : public GameTimer {

    float   m_startTime;
    float   m_lastTime;
    double  m_elapsed;

public:

    AndroidTimer() : m_startTime(0), m_lastTime(0), m_elapsed(0) {}
    virtual ~AndroidTimer() noexcept {}
    virtual void update() override;
    virtual double getElapsedSeconds() override;
};
