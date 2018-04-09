//
//  AIObject.cpp
//  PixFight
//
//  Created by Marcin Małysz on 06/01/2018.
//  Copyright © 2018 Marcin Małysz. All rights reserved.
//

#include "AIObject.hpp"

AIObject::AIObject(UNITTYPE type) {

    exectutorId = -1;
    prorities.clear();
    firstPriorityTask = nullptr;
    secondPriorityTask = nullptr;
    AIMode = UNIT_NONE;
    ownType = type;

    switch (type) {
        case M_INFANTRY:{
            int val = AI_CAPTURE_BASES;
            prorities.push_back(val);

            val = AI_DEFENSE_BASES;
            prorities.push_back(val);

            val = AI_ATTACK_ENEMY;
            prorities.push_back(val);

            val = AI_STAY_UNIT;
            prorities.push_back(val);
        }
            break;

        case M_BAZOOKA:{

            int val = AI_DEFENSE_BASES;
            prorities.push_back(val);

            val = AI_CAPTURE_BASES;
            prorities.push_back(val);

            val = AI_ATTACK_ENEMY;
            prorities.push_back(val);

            val = AI_STAY_UNIT;
            prorities.push_back(val);
        }
            break;

        case M_JEEP:{
            int val = AI_DEFENSE_BASES;
            prorities.push_back(val);

            val = AI_ATTACK_ENEMY;
            prorities.push_back(val);

            val = AI_REPAIR_UNIT;
            prorities.push_back(val);

            val = AI_STAY_UNIT;
            prorities.push_back(val);
        }
            break;

        case M_LTANK:{
            int val = AI_DEFENSE_BASES;
            prorities.push_back(val);

            val = AI_ATTACK_ENEMY;
            prorities.push_back(val);

            val = AI_REPAIR_UNIT;
            prorities.push_back(val);

            val = AI_STAY_UNIT;
            prorities.push_back(val);
        }
            break;

        case M_ARTILLERY:{
            int val = AI_DEFENSE_BASES;
            prorities.push_back(val);

            val = AI_ATTACK_ENEMY;
            prorities.push_back(val);

            val = AI_REPAIR_UNIT;
            prorities.push_back(val);

            val = AI_STAY_UNIT;
            prorities.push_back(val);
        }
            break;

        case M_BASEUNIT:{
            int val = AI_BUILD_UNIT;
            prorities.push_back(val);

            val = AI_STAY_UNIT;
            prorities.push_back(val);
        }
            break;

        default:
            break;
    }
}

bool AIObject::assignTask(AITask *Task) {

    if (firstPriorityTask == nullptr) {

        firstPriorityTask = Task;
        return true;
    }
    else if (secondPriorityTask == nullptr) {

        secondPriorityTask = Task;
        return true;
    }

    return false;
}
