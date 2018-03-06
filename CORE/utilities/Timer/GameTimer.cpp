//
//  GameTimer.cpp
//  PixFight
//
//  Created by Marcin Małysz on 17/01/2018.
//  Copyright © 2018 Marcin Małysz. All rights reserved.
//

#include "GameTimer.hpp"

#ifdef __linux__
    #include "AndroidTimer.hpp"
#endif

#ifdef _WIN32
	#include "WindowsTimer.hpp"
#endif

#ifdef __APPLE__
    #include "DarwinTimer.hpp"
#endif

#ifdef __ANDROID__
    #include "AndroidTimer.hpp"
#endif

extern GameTimer *getPlatformTimerInstance() {

#ifdef __linux__
    return new AndroidTimer();
#endif

#ifdef _WIN32
	return new WindowsTimer();
#endif

#ifdef __APPLE__
    return new DarwinTimer();
#endif

#ifdef __ANDROID__
    return new AndroidTimer();
#endif

    return nullptr;
}
