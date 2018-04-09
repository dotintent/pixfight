//
//  AIBaseInclude.hpp
//  PixFight
//
//  Created by Marcin Małysz on 06/01/2018.
//  Copyright © 2018 Marcin Małysz. All rights reserved.
//

#pragma once

#include "EnumOptionMacro.hpp"

#define MAX_PRIORITY 7.0

typedef PF_ENUM(uint32_t, AI_ACTIONS_PRIORITY) {
    AI_STAY_UNIT = 6,
    AI_REPAIR_UNIT = 5,
    AI_DEFENSE_BASES = 4,
    AI_BUILD_UNIT = 3,
    AI_ATTACK_ENEMY = 2,
    AI_CAPTURE_BASES = 1,
    AI_DO_NOTHING_PRIOR = 7
};
