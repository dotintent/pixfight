#include "DarwinTimer.hpp"

#include <mach/mach_time.h>

void DarwinTimer::update() {

    this->m_lastTime = mach_absolute_time();

    uint64_t difference = m_lastTime - m_startTime;
    static double conversion = 0.0;

    if ( conversion == 0.0 )
    {
        mach_timebase_info_data_t info;
        kern_return_t err = mach_timebase_info( &info );

        if( err == 0  )
            conversion = 1e-9 * (double) info.numer / (double) info.denom;
    }

    m_elapsed = conversion * (double) difference;

    this->m_startTime = this->m_lastTime;
}

double DarwinTimer::getElapsedSeconds() {
		
    return m_elapsed;
}

