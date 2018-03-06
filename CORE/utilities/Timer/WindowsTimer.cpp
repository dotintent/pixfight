#include "WindowsTimer.hpp"

void WindowsTimer::update() {

	this->m_lastTime = milliseconds_now();
  
	if (_use_qpc) {

		m_elapsed = (m_lastTime - m_startTime) / _frequency.QuadPart;
	}
	else {

		m_elapsed = m_lastTime - m_startTime;
	}

    this->m_startTime = this->m_lastTime;
}

double WindowsTimer::getElapsedSeconds() {
		
    return m_elapsed;
}

long long WindowsTimer::milliseconds_now() {

	if (_use_qpc) {
	
		LARGE_INTEGER now;
		QueryPerformanceCounter(&now);
		return now.QuadPart;
	}
	else {

		return GetTickCount();
	}
}