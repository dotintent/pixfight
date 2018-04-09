//
//  AITaskGenerator.hpp
//  PixFight
//
//  Created by Marcin Małysz on 06/01/2018.
//  Copyright © 2018 Marcin Małysz. All rights reserved.
//

#pragma once

#include "AITask.hpp"
#include "AIObject.hpp"

class GameUnit;
class GameBase;

class AITaskGenerator {

public:

    AITaskGenerator() : berserk(false) {}
    ~AITaskGenerator() noexcept {}

    bool berserk;

    void generatePossibleTasks(AIObject *current);
    void calculateScore(const std::vector<GameUnit *> & units, const std::vector<GameBase *> & bases);
    void sortTasks();
    void clearTasks();

private:

    void assignTasksToObject(const std::vector<AITask *> & tasks, AIObject *object);

    struct AIAction {

        AIAction() : owner(nullptr) { }
        ~AIAction() {

            for (auto t : taskToAssign) { delete t; }
            taskToAssign.clear();
            std::vector<AITask *>().swap(taskToAssign);
        }
        
        AIObject *owner;
        std::vector<AITask *> taskToAssign;
    };

    std::vector<AIAction *> _actions;
};
