//
//  AIObject.hpp
//  PixFight
//
//  Created by Marcin Małysz on 06/01/2018.
//  Copyright © 2018 Marcin Małysz. All rights reserved.
//

#pragma once

#include "AITask.hpp"
#include "GameEnums.hpp"
#include <vector>

class AIObject {

public:

    AIObject(UNITTYPE type);
    ~AIObject() {}

    std::vector<int> prorities;
    int exectutorId;

    AITask *firstPriorityTask;
    AITask *secondPriorityTask;

    UNITMODE AIMode;
    UNITTYPE ownType;

    void assignExecutor(const int & ID) { exectutorId = ID; }
    bool assignTask(AITask *Task);
};
