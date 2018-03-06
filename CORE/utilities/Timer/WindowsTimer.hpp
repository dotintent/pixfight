#pragma once

#include "GameTimer.hpp"
#include <windows.h>

class WindowsTimer : public GameTimer {

	LARGE_INTEGER _frequency;
	BOOL _use_qpc;

    double m_startTime;
    double m_lastTime;
    double m_elapsed;
    
public:

	WindowsTimer() : m_startTime(0), m_lastTime(0), m_elapsed(0) {
		
		_use_qpc = QueryPerformanceFrequency(&_frequency);
	}

    virtual ~WindowsTimer() noexcept {}
    virtual void update() override;
	virtual double getElapsedSeconds() override;

private:

	long long milliseconds_now();
};

