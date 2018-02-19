//
//  AITask.hpp
//  PixFight
//
//  Created by Marcin Małysz on 06/01/2018.
//  Copyright © 2018 Marcin Małysz. All rights reserved.
//

#pragma once

#include "AIBaseInclude.hpp"

class AITask {

public:

    AITask() : score(0), objectiveId(-1), Priority(AI_DO_NOTHING_PRIOR) {}

    float score;
    int objectiveId;

    AI_ACTIONS_PRIORITY Priority;
};
