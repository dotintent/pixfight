#pragma once

#include "GameTimer.hpp"

class DarwinTimer : public GameTimer {

    uint64_t m_startTime;
    uint64_t m_lastTime;
    double   m_elapsed;
    
public:

    DarwinTimer() : m_startTime(0), m_lastTime(0), m_elapsed(0) {}
    virtual ~DarwinTimer() noexcept {}
    virtual void update() override;
	virtual double getElapsedSeconds() override;
};

